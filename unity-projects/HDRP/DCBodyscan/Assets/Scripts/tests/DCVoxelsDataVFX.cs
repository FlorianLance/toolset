


/***********************************************************************************
** MIT License                                                                    **
** Copyright (c) [2024] [Florian Lance]                                           **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
************************************************************************************/

// system
using System.Collections.Generic;
using System.Collections;
using System.IO;

// unity
using UnityEngine;
using UnityEngine.VFX;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace BS {



//#if UNITY_EDITOR

//    [CustomEditor(typeof(DCDataVFX))]
//    public class DCDataVFXEditor : Editor {

//        private float durationS = 10f;
//        private int nbTriggering = 1000;
//        private int particleCountPerTriggering = 25;
//        private float particleLifeTimeS = 12f;
//        private bool reduceCloudVisibilityWhileGenerating = false;
//        private Color forceColor = Color.blue;

//        private DCDataVFX dataVFX { get { return target as DCDataVFX; } }
//        public string dataVFXSettingsJsonFilePath = "./data_vfx_settings.json";

//        public override bool RequiresConstantRepaint() {
//            return true;
//        }

//        public override void OnInspectorGUI() {

//            EditorGUILayout.LabelField("[PARTICLES]");            

//            EditorGUILayout.BeginHorizontal();
//            EditorGUILayout.LabelField("Duration (S):");
//            durationS = EditorGUILayout.Slider(durationS, 0.1f, 100f);
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.BeginHorizontal();
//            EditorGUILayout.LabelField("Triggering count:");
//            nbTriggering = EditorGUILayout.IntSlider(nbTriggering, 1, 100000);
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.BeginHorizontal();
//            EditorGUILayout.LabelField("Particle count per trighgering:");
//            particleCountPerTriggering = EditorGUILayout.IntSlider(particleCountPerTriggering, 1, 1000);
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.BeginHorizontal();
//            EditorGUILayout.LabelField("Particle lifetime (S):");
//            particleLifeTimeS = EditorGUILayout.Slider(particleLifeTimeS, 0.1f, 120f);
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.BeginHorizontal();
//            EditorGUILayout.LabelField("Reduce cloud visibility while generating:");
//            reduceCloudVisibilityWhileGenerating = EditorGUILayout.Toggle(reduceCloudVisibilityWhileGenerating);
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.BeginHorizontal();
//            EditorGUILayout.LabelField("Force color:");
//            forceColor = EditorGUILayout.ColorField(forceColor);
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.Separator();

//            EditorGUILayout.BeginHorizontal();                      
//            if (GUILayout.Button("Generate")) {
//                dataVFX.generate_particles(durationS, nbTriggering, particleCountPerTriggering, particleLifeTimeS, reduceCloudVisibilityWhileGenerating);
//            }
//            if (GUILayout.Button("Restore visibility")) {
//                dataVFX.restore_visibility(durationS, nbTriggering);                
//            }
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.BeginHorizontal();
//            if (GUILayout.Button("Fade to color")) {
//                dataVFX.fade_to_color(durationS, forceColor);
//            }
//            if (GUILayout.Button("Restore color")) {
//                dataVFX.restore_color();
//            }
//            EditorGUILayout.EndHorizontal();

//            EditorGUILayout.Separator();
//            EditorGUILayout.Separator();

//            EditorGUILayout.LabelField("[JSON]");

//            //GUILayout.TextArea("Type the base path for the JSON file (id of the grabber and JSON ext will be added automatically");
//            dataVFXSettingsJsonFilePath = GUILayout.TextField(dataVFXSettingsJsonFilePath);
//            if (GUILayout.Button("Save DataVFX settings to JSON file")) {                
//                try {
//                    File.WriteAllText(dataVFXSettingsJsonFilePath, JsonUtility.ToJson(dataVFX.settings));
//                } catch (System.Exception ex) {
//                    UnityEngine.Debug.LogError(string.Format("[DCDataVFXEditor::OnInspectorGUI] Cannot save JSON settings, error: [{0}] ", ex.Message));
//                }
//            }

//            EditorGUILayout.Separator();
//            base.OnInspectorGUI();
//        }
//    }
//#endif

    public class DCVoxelsDataVFX : MonoBehaviour {

        public VisualEffectAsset sharedVisualEffectAsset = null;        
        public DCCloudDisplaySettings settings = new DCCloudDisplaySettings();

        public List<GameObject> voxelsGroupsGO = new List<GameObject>();

        private NativeDLLVec3 positionsNativeBuffer = null;
        private NativeDLLVec3 colorsNativeBuffer = null;
        private List<GraphicsBuffer> positionsGBuffers = new List<GraphicsBuffer>();
        private List<GraphicsBuffer> colorsGBuffers = new List<GraphicsBuffer>();

        private Mesh paraboloidMesh = null;


        private int sizeMaxVoxelGroup = 100000;
        public void initialize(int maxFrameSize) {

            if (sharedVisualEffectAsset == null) {
                UnityEngine.Debug.LogError("[DCVoxelsDataVFX::initialize] No visual effect asset defined, initialization canceled.");
                return;
            }

            int nbClouds = maxFrameSize / sizeMaxVoxelGroup;
            int rest     = maxFrameSize % sizeMaxVoxelGroup;
            if (rest != 0) {
                nbClouds++;
            }

            UnityEngine.Debug.Log("maxCloudSize " + maxFrameSize + " nbClouds " + nbClouds + " rest " + rest);


            if (nbClouds <= 0) {
                UnityEngine.Debug.LogError("[DCVoxelsDataVFX::initialize] Invalid cloud number, initialization canceled.");
                return;
            }

            clean();

            // init native buffers
            positionsNativeBuffer   = new BS.NativeDLLVec3(maxFrameSize);
            colorsNativeBuffer      = new BS.NativeDLLVec3(maxFrameSize);

            // create gameobjects
            voxelsGroupsGO          = new List<GameObject>(nbClouds);
            positionsGBuffers       = new List<GraphicsBuffer>(nbClouds);
            colorsGBuffers          = new List<GraphicsBuffer>(nbClouds);

            for (int idC = 0; idC < nbClouds; ++idC) {

                // create GO
                var voxelsGroup = new GameObject();
                voxelsGroup.transform.SetParent(transform);
                voxelsGroup.name = string.Format("Voxels group {0}", idC);
                voxelsGroup.transform.localPosition = Vector3.zero;
                voxelsGroup.transform.localRotation = Quaternion.identity;
                voxelsGroup.transform.localScale    = Vector3.one;

                // add visual effect
                var ve = voxelsGroup.AddComponent<VisualEffect>();
                ve.visualEffectAsset = sharedVisualEffectAsset;

                // create graphics buffers
                var posGB = new GraphicsBuffer(GraphicsBuffer.Target.Structured, sizeMaxVoxelGroup, 3 * sizeof(float));
                var colGB = new GraphicsBuffer(GraphicsBuffer.Target.Structured, sizeMaxVoxelGroup, 3 * sizeof(float));

                // set visual effect associated graphic buffers
                ve.SetGraphicsBuffer("Positions", posGB);
                ve.SetGraphicsBuffer("Colors", colGB);
                paraboloidMesh = ProceduralMeshes.low_poly_paraboloid(0.6f, 0.6f);
                if (paraboloidMesh != null) {
                    ve.SetMesh("InstanceMesh", paraboloidMesh);
                }

                positionsGBuffers.Add(posGB);
                colorsGBuffers.Add(colGB);

                voxelsGroupsGO.Add(voxelsGroup);
            }
        }

        public void load_settings_file(string settingsFilePath) {

            if (!File.Exists(settingsFilePath)) {
                UnityEngine.Debug.LogWarning(string.Format("[DCVoxelsDataVFX::load_settings_file] File with path [{0}] doesn't exist. ", settingsFilePath));
                return;
            }

            try {
                string jsonContent = File.ReadAllText(settingsFilePath);
                var newSettings = JsonUtility.FromJson<DCCloudDisplaySettings>(jsonContent);
                if (newSettings != null) {
                    settings = newSettings;
                } else {
                    UnityEngine.Debug.LogError(string.Format("[DCVoxelsDataVFX::load_settings_file] Cannot convert text to JSON from file with path [{0}] with text: [{1}] ", settingsFilePath, jsonContent));
                }

            } catch (System.Exception ex) {
                UnityEngine.Debug.LogError(string.Format("[DCVoxelsDataVFX::load_settings_file] Cannot read file with path [{0}], error: [{1}] ", settingsFilePath, ex.Message));
            }
        }

        public void update_shader_from_display_parameters() {

            // set shader parameters
            for (int idC = 0; idC < voxelsGroupsGO.Count; ++idC) {
                var visualEffect = voxelsGroupsGO[idC].GetComponent<VisualEffect>();
                //visualEffect.SetBool("EnableBackFaceCulling", settings.enableBackFaceCulling);
                visualEffect.SetVector4("Tint", settings.tint);
                visualEffect.SetFloat("OctogonCropFactor", settings.octogonCropFactor);
                visualEffect.SetFloat("ColorFactor", settings.colorFactor);
                visualEffect.SetFloat("ParticleSize", settings.particleSize);
                visualEffect.enabled = settings.display;
            }
        }

        public void update_data(int nbPoints) {

            if ((nbPoints >= 0) && settings.update) {

                //UnityEngine.Debug.Log("update_data : " + nbPoints);
                int nbClouds = nbPoints / sizeMaxVoxelGroup;
                int rest = nbPoints % sizeMaxVoxelGroup;
                if (rest != 0) {
                    nbClouds++;
                }

                int totalNbPoints = nbPoints + rest;
                //UnityEngine.Debug.Log("nbClouds : " + nbClouds + " nbPoints " + nbPoints + " totalNbPoints " + totalNbPoints);
                for (int idC = 0; idC < nbClouds; ++idC) {

                    int currentNbPoints = (totalNbPoints >= sizeMaxVoxelGroup) ? sizeMaxVoxelGroup : totalNbPoints;
                    totalNbPoints -= currentNbPoints;

                    //UnityEngine.Debug.Log("currentNbPoints: " + currentNbPoints + " totalNbPoints " + totalNbPoints + " idc " + idC + " idC * sizeMaxVoxelGroup " + idC * sizeMaxVoxelGroup);
                    positionsGBuffers[idC].SetData(positionsNativeBuffer.native, idC * sizeMaxVoxelGroup, 0, currentNbPoints);
                    colorsGBuffers[idC].SetData(colorsNativeBuffer.native, idC * sizeMaxVoxelGroup, 0, currentNbPoints);
                    voxelsGroupsGO[idC].GetComponent<VisualEffect>().SetInt("NbPoints", currentNbPoints);

                    if (nbPoints <= 0) {
                        break;
                    }
                }
            }
        }

        public NativeDLLVec3 positions_native_buffer() {
            return positionsNativeBuffer;
        }

        public NativeDLLVec3 colors_native_buffer() {
            return colorsNativeBuffer;
        }

        public int voxels_group_count() {
            return voxelsGroupsGO.Count;
        }

        public void set_tint(UnityEngine.Color color) {
            settings.tint = color;
            for (int idC = 0; idC < voxelsGroupsGO.Count; ++idC) {
                voxelsGroupsGO[idC].GetComponent<VisualEffect>().SetVector4("Tint", color);
            }
        }
        public void set_display_state(bool state) {
            settings.display = state;
            for (int idC = 0; idC < voxelsGroupsGO.Count; ++idC) {
                voxelsGroupsGO[idC].GetComponent<VisualEffect>().enabled = state;
            }
        }
        public void set_update_state(bool state) {
            settings.update = state;
        }

        public void set_octogon_crop_factor(float factor) {
            settings.octogonCropFactor = factor;
            for (int idC = 0; idC < voxelsGroupsGO.Count; ++idC) {
                voxelsGroupsGO[idC].GetComponent<VisualEffect>().SetFloat("OctogonCropFactor", factor);
            }
        }

        public void set_color_factor(float factor) {
            settings.colorFactor = factor;
            for (int idC = 0; idC < voxelsGroupsGO.Count; ++idC) {
                voxelsGroupsGO[idC].GetComponent<VisualEffect>().SetFloat("ColorFactor", factor);
            }
        }

        public void set_particle_size(int idCloud, float size) {
            settings.particleSize = size;
            for (int idC = 0; idC < voxelsGroupsGO.Count; ++idC) {
                voxelsGroupsGO[idC].GetComponent<VisualEffect>().SetFloat("ParticleSize", size);
            }
        }

        private void clean() {

            // destroy gameobjects
            for (int ii = 0; ii < voxelsGroupsGO.Count; ++ii) {
                Destroy(voxelsGroupsGO[ii]);
            }
            voxelsGroupsGO.Clear();

            // clean graphics buffers
            foreach (var posGB in positionsGBuffers) {
                posGB.Dispose();
            }
            positionsGBuffers.Clear();
            foreach (var colGB in colorsGBuffers) {
                colGB.Dispose();
            }
            colorsGBuffers.Clear();

            // clean native buffers
            if (positionsNativeBuffer != null) {
                positionsNativeBuffer.clean();
                positionsNativeBuffer = null;
            }
            if (colorsNativeBuffer != null) {
                colorsNativeBuffer.clean();
                colorsNativeBuffer = null;
            }
        }

        private void OnDestroy() {
            clean();
        }
    }
}