


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



#if UNITY_EDITOR

    [CustomEditor(typeof(DCDataVFX))]
    public class DCDataVFXEditor : Editor {

        private float durationS = 10f;
        private int nbTriggering = 1000;
        private int particleCountPerTriggering = 25;
        private float particleLifeTimeS = 12f;
        private bool reduceCloudVisibilityWhileGenerating = false;
        private Color forceColor = Color.blue;

        private DCDataVFX dataVFX { get { return target as DCDataVFX; } }
        public string dataVFXSettingsJsonFilePath = "./data_vfx_settings.json";

        public override bool RequiresConstantRepaint() {
            return true;
        }

        public override void OnInspectorGUI() {

            EditorGUILayout.LabelField("[PARTICLES]");            

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Duration (S):");
            durationS = EditorGUILayout.Slider(durationS, 0.1f, 100f);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Triggering count:");
            nbTriggering = EditorGUILayout.IntSlider(nbTriggering, 1, 100000);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Particle count per trighgering:");
            particleCountPerTriggering = EditorGUILayout.IntSlider(particleCountPerTriggering, 1, 1000);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Particle lifetime (S):");
            particleLifeTimeS = EditorGUILayout.Slider(particleLifeTimeS, 0.1f, 120f);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Reduce cloud visibility while generating:");
            reduceCloudVisibilityWhileGenerating = EditorGUILayout.Toggle(reduceCloudVisibilityWhileGenerating);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Force color:");
            forceColor = EditorGUILayout.ColorField(forceColor);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.Separator();

            EditorGUILayout.BeginHorizontal();                      
            if (GUILayout.Button("Generate")) {
                dataVFX.generate_particles(durationS, nbTriggering, particleCountPerTriggering, particleLifeTimeS, reduceCloudVisibilityWhileGenerating);
            }
            if (GUILayout.Button("Restore visibility")) {
                dataVFX.restore_visibility(durationS, nbTriggering);                
            }
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            if (GUILayout.Button("Fade to color")) {
                dataVFX.fade_to_color(durationS, forceColor);
            }
            if (GUILayout.Button("Restore color")) {
                dataVFX.restore_color();
            }
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.Separator();
            EditorGUILayout.Separator();

            EditorGUILayout.LabelField("[JSON]");

            //GUILayout.TextArea("Type the base path for the JSON file (id of the grabber and JSON ext will be added automatically");
            dataVFXSettingsJsonFilePath = GUILayout.TextField(dataVFXSettingsJsonFilePath);
            if (GUILayout.Button("Save DataVFX settings to JSON file")) {                
                try {
                    File.WriteAllText(dataVFXSettingsJsonFilePath, JsonUtility.ToJson(dataVFX.settings));
                } catch (System.Exception ex) {
                    UnityEngine.Debug.LogError(string.Format("[DCDataVFXEditor::OnInspectorGUI] Cannot save JSON settings, error: [{0}] ", ex.Message));
                }
            }

            EditorGUILayout.Separator();
            base.OnInspectorGUI();
        }
    }
#endif

    public class DCDataVFX : MonoBehaviour {

        public VisualEffectAsset sharedVisualEffectAsset = null;
        public List<GameObject> cloudsGO = new List<GameObject>();
        public DCDataVFXSettings settings = new DCDataVFXSettings();

        private List<NativeDLLVec3> positionsNativeBuffers = new List<BS.NativeDLLVec3>();
        private List<NativeDLLVec3> colorsNativeBuffers = new List<BS.NativeDLLVec3>();
        private List<NativeDLLVec3> normalsNativeBuffers = new List<BS.NativeDLLVec3>();
        private List<GraphicsBuffer> positionsGBuffers = new List<GraphicsBuffer>();
        private List<GraphicsBuffer> colorsGBuffers = new List<GraphicsBuffer>();
        private List<GraphicsBuffer> normalsGBuffers = new List<GraphicsBuffer>();
        private Mesh paraboloidMesh = null;

        private IEnumerator fadeToColorCo = null;
        private IEnumerator generateParticlesCo = null;
        private IEnumerator solveCO = null;

        public void initialize(int nbClouds, int sizeMaxCloud = 640 * 576) {

            if (sharedVisualEffectAsset == null) {
                UnityEngine.Debug.LogError("[DCDataVFX::initialize] No visual effect asset defined, initialization canceled.");
                return;
            }

            if (nbClouds <= 0) {
                UnityEngine.Debug.LogError("[DCDataVFX::initialize] Invalid cloud number, initialization canceled.");
                return;
            }

            clean();

            cloudsGO = new List<GameObject>(nbClouds);
            settings = new DCDataVFXSettings();
            settings.cloudsDisplayS = new List<DCCloudDisplaySettings>(nbClouds);

            // create new graphic buffers list
            positionsGBuffers = new List<GraphicsBuffer>(nbClouds);
            colorsGBuffers = new List<GraphicsBuffer>(nbClouds);
            normalsGBuffers = new List<GraphicsBuffer>(nbClouds);

            // create new native buffers list
            positionsNativeBuffers = new List<NativeDLLVec3>(nbClouds);
            colorsNativeBuffers = new List<NativeDLLVec3>(nbClouds);
            normalsNativeBuffers = new List<NativeDLLVec3>(nbClouds);

            for (int idC = 0; idC < nbClouds; ++idC) {

                // create GO
                var cloudGO = new GameObject();
                cloudGO.transform.SetParent(transform);
                cloudGO.name = string.Format("Cloud {0}", idC);
                cloudGO.transform.localPosition = Vector3.zero;
                cloudGO.transform.localRotation = Quaternion.identity;
                cloudGO.transform.localScale = Vector3.one;

                settings.cloudsDisplayS.Add(new DCCloudDisplaySettings());

                // create graphics buffers
                var posGB = new GraphicsBuffer(GraphicsBuffer.Target.Structured, sizeMaxCloud, 3 * sizeof(float));
                var colGB = new GraphicsBuffer(GraphicsBuffer.Target.Structured, sizeMaxCloud, 3 * sizeof(float));
                var normGB = new GraphicsBuffer(GraphicsBuffer.Target.Structured, sizeMaxCloud, 3 * sizeof(float));

                // add visual effect
                var ve = cloudGO.AddComponent<VisualEffect>();
                ve.visualEffectAsset = sharedVisualEffectAsset;

                // set visual effect associated graphic buffers
                ve.SetGraphicsBuffer("Positions", posGB);
                ve.SetGraphicsBuffer("Colors", colGB);
                ve.SetGraphicsBuffer("Normals", normGB);
                paraboloidMesh = ProceduralMeshes.low_poly_paraboloid(0.6f, 0.6f);
                if (paraboloidMesh != null) {
                    ve.SetMesh("InstanceMesh", paraboloidMesh);
                }

                positionsGBuffers.Add(posGB);
                colorsGBuffers.Add(colGB);
                normalsGBuffers.Add(normGB);

                // create native arrays
                positionsNativeBuffers.Add(new BS.NativeDLLVec3(sizeMaxCloud));
                colorsNativeBuffers.Add(new BS.NativeDLLVec3(sizeMaxCloud));
                normalsNativeBuffers.Add(new BS.NativeDLLVec3(sizeMaxCloud));

                cloudsGO.Add(cloudGO);
            }
        }

        public void load_settings_file(string settingsFilePath) {

            if (!File.Exists(settingsFilePath)) {
                UnityEngine.Debug.LogWarning(string.Format("[DCDataVFX::load_settings_file] File with path [{0}] doesn't exist. ", settingsFilePath));
                return;
            }

            try {
                string jsonContent = File.ReadAllText(settingsFilePath);
                var newSettings = JsonUtility.FromJson<DCDataVFXSettings>(jsonContent);
                if (newSettings != null) {
                    if(newSettings.cloudsDisplayS.Count == settings.cloudsDisplayS.Count) {
                        settings = newSettings;
                    } else{
                        UnityEngine.Debug.LogWarning(string.Format("[DCDataVFX::load_settings_file] Inconsistencies bewteen current cloud settings and loaded file number of clouds settings with path [{0}] with text: [{1}]", settingsFilePath, jsonContent));
                    }
                } else {
                    UnityEngine.Debug.LogError(string.Format("[DCDataVFX::load_settings_file] Cannot convert text to JSON from file with path [{0}] with text: [{1}] ", settingsFilePath, jsonContent));
                }

            } catch (System.Exception ex) {
                UnityEngine.Debug.LogError(string.Format("[DCDataVFX::load_settings_file] Cannot read file with path [{0}], error: [{1}] ", settingsFilePath, ex.Message));
            }
        }

        public void update_shader_from_display_parameters() {

            if (settings.cloudsDisplayS.Count != cloudsGO.Count) {
                UnityEngine.Debug.LogError("Invalid DataVFX settings file.");
                return;
            }

            // set shader parameters
            for(int idC = 0; idC < settings.cloudsDisplayS.Count; ++idC) {
                var currentS = settings.cloudsDisplayS[idC];
                var visualEffect = cloudsGO[idC].GetComponent<VisualEffect>();
                visualEffect.SetBool("EnableBackFaceCulling", currentS.enableBackFaceCulling);
                visualEffect.SetVector4("Tint", currentS.tint);
                visualEffect.SetFloat("OctogonCropFactor", currentS.octogonCropFactor);
                visualEffect.SetFloat("ColorFactor", currentS.colorFactor);
                visualEffect.SetFloat("ParticleSize", currentS.particleSize);
                visualEffect.enabled = currentS.display;
            }

            if (settings.applyParentTransform) {
                transform.localPosition = settings.parentTransform.position;
                transform.localRotation = settings.parentTransform.rotation;
                transform.localScale    = settings.parentTransform.scale;
            }
        }

        public void update_data(int idCloud, int nbPoints) {

            if (idCloud >= settings.cloudsDisplayS.Count) {
                return;
            }

            if ((nbPoints >= 0) && settings.cloudsDisplayS[idCloud].update) {

                // set data
                colorsGBuffers[idCloud].SetData(colorsNativeBuffers[idCloud].native, 0, 0, nbPoints);
                positionsGBuffers[idCloud].SetData(positionsNativeBuffers[idCloud].native, 0, 0, nbPoints);
                normalsGBuffers[idCloud].SetData(normalsNativeBuffers[idCloud].native, 0, 0, nbPoints);

                // update number of points visible
                cloudsGO[idCloud].GetComponent<VisualEffect>().SetInt("NbPoints", nbPoints);
            }
        }

        public NativeDLLVec3 positions_native_buffer(int idCloud) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                return positionsNativeBuffers[idCloud];
            }
            return null;
        }

        public NativeDLLVec3 colors_native_buffer(int idCloud) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                return colorsNativeBuffers[idCloud];
            }
            return null;
        }

        public NativeDLLVec3 normals_native_buffer(int idCloud) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                return normalsNativeBuffers[idCloud];
            }
            return null;
        }

        public int clouds_count() {
            return cloudsGO.Count;
        }

        public void set_tint(int idCloud, UnityEngine.Color color) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                settings.cloudsDisplayS[idCloud].tint = color;
                cloudsGO[idCloud].GetComponent<VisualEffect>().SetVector4("Tint", color);
            }
        }
        public void set_display_state(int idCloud, bool state) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                settings.cloudsDisplayS[idCloud].display = state;
                cloudsGO[idCloud].GetComponent<VisualEffect>().enabled = state;
            }
        }
        public void set_update_state(int idCloud, bool state) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                settings.cloudsDisplayS[idCloud].update = state;
            }
        }

        public void set_octogon_crop_factor(int idCloud, float factor) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                settings.cloudsDisplayS[idCloud].octogonCropFactor = factor;
                cloudsGO[idCloud].GetComponent<VisualEffect>().SetFloat("OctogonCropFactor", factor);
            }
        }

        public void set_color_factor(int idCloud, float factor) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                settings.cloudsDisplayS[idCloud].colorFactor = factor;
                cloudsGO[idCloud].GetComponent<VisualEffect>().SetFloat("ColorFactor", factor);
            }
        }

        public void set_particle_size(int idCloud, float size) {
            if (idCloud < settings.cloudsDisplayS.Count) {
                settings.cloudsDisplayS[idCloud].particleSize = size;
                cloudsGO[idCloud].GetComponent<VisualEffect>().SetFloat("ParticleSize", size);
            }
        }

        public IEnumerator fade_to_color_co(float durationS, Color color) {

            float offestTime = durationS / 1000f;

            foreach (var cloud in cloudsGO) {
                var vfx = cloud.GetComponent<VisualEffect>();
                vfx.SetFloat("ForceColorFactor", 0f);
                vfx.SetVector4("ForceColor", color);
            }

            for (int ii = 0; ii < 1000; ++ii) {

                foreach (var cloud in cloudsGO) {
                    var vfx = cloud.GetComponent<VisualEffect>();
                    vfx.SetFloat("ForceColorFactor", ii / 1000f);
                }
                yield return new WaitForSeconds(offestTime);
            }            
        }


        public IEnumerator generate_particles_co(float durationS, int nbTriggerings, int particleCountPerTriggering, float particleLifeTimeS, bool reduceCloudVisibiltyWhileGenerating) {

            var eventId = Shader.PropertyToID("GenerateParticles");
            foreach (var cloud in cloudsGO) {
                var vfx = cloud.GetComponent<VisualEffect>();
                vfx.SetFloat("ParticuleLifeTime", particleLifeTimeS);
            }

            for (int ii = 0; ii < nbTriggerings; ++ii) {
                foreach (var cloud in cloudsGO) {
                    var vfx = cloud.GetComponent<VisualEffect>();
                    if (reduceCloudVisibiltyWhileGenerating) {
                        vfx.SetInt("Visibility", (int)(1000 - 1000 * (1f * ii / nbTriggerings)));
                    }
                    vfx.SetInt("ParticuleCountCreation", particleCountPerTriggering);
                    vfx.SendEvent(eventId);
                }
                yield return new WaitForSeconds(durationS / nbTriggerings);
            }
        }

        public IEnumerator restore_visibility_co(float durationS, int nbTriggerings) {

            for (int ii = 0; ii < nbTriggerings; ++ii) {
                foreach (var cloud in cloudsGO) {
                    var vfx = cloud.GetComponent<VisualEffect>();
                    vfx.SetInt("Visibility", (int)(1000 * (1f * ii / nbTriggerings)));
                }
                yield return new WaitForSeconds(durationS / nbTriggerings);
            }
        }


        public void fade_to_color(float durationS, Color color) {
            fadeToColorCo = fade_to_color_co(durationS, color);
            StartCoroutine(fadeToColorCo);
        }


        public void generate_particles(float durationS, int nbTriggerings, int particleCountPerTriggering, float particleLifeTimeS, bool reduceCloudVisibiltyWhileGenerating) {
            generateParticlesCo = generate_particles_co(durationS, nbTriggerings, particleCountPerTriggering, particleLifeTimeS, reduceCloudVisibiltyWhileGenerating);
            StartCoroutine(generateParticlesCo);
        }

        public void restore_visibility(float durationS, int nbTriggerings) {
            solveCO = restore_visibility_co(durationS, nbTriggerings);
            StartCoroutine(solveCO);
        }

        public void restore_color() {
            foreach (var cloud in cloudsGO) {
                var vfx = cloud.GetComponent<VisualEffect>();
                vfx.SetFloat("ForceColorFactor", 0f);
            }
        }

        private void clean() {

            // destroy gameobjects
            for (int ii = 0; ii < cloudsGO.Count; ++ii) {
                Destroy(cloudsGO[ii]);
            }
            cloudsGO.Clear();

            // clean graphics buffers
            foreach (var posGB in positionsGBuffers) {
                posGB.Dispose();
            }
            foreach (var colGB in colorsGBuffers) {
                colGB.Dispose();
            }
            foreach (var normGB in normalsGBuffers) {
                normGB.Dispose();
            }

            // clean native buffers
            foreach (var posNB in positionsNativeBuffers) {
                posNB.clean();
            }
            foreach (var colorNB in colorsNativeBuffers) {
                colorNB.clean();
            }
            foreach (var normNb in normalsNativeBuffers) {
                normNb.clean();
            }
        }

        private void OnDestroy() {
            clean();
        }
    }
}