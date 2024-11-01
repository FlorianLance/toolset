
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
using System.Collections;

// unity
using UnityEngine.VFX;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace BS {


#if UNITY_EDITOR

    [CustomEditor(typeof(VFXParticleProfile1))]
    public class VFXParticleProfile1Editor : Editor {

        private VFXParticleProfile1 profile { get { return target as VFXParticleProfile1; } }

        public override bool RequiresConstantRepaint() {
            return true;
        }

        public override void OnInspectorGUI() {

            if (GUILayout.Button("Start")) {
                UnityEngine.Debug.Log("START");
                profile.start_events();
            }
            EditorGUILayout.Separator();
            base.OnInspectorGUI();
        }
    }
#endif


    public class VFXParticleProfile1 : VFXBaseProfile {

        public float durationS = 15f;
        public float particleLifeTimeS = 5f;
        public int nbTriggerings = 50;
        public bool reduceCloudVisibiltyWhileGenerating = true;
        public int particleCountPerTriggering = 1000;

        protected override IEnumerator profile_co() {

            // retrieve event id from name
            var eventId = Shader.PropertyToID("Event2"); 

            // // define particle lifetime and particle count before triggering event
            foreach (var cloud in dataVFX.cloudsGO) {
                var vfx = cloud.GetComponent<VisualEffect>();
                vfx.SetFloat("ParticuleLifeTime", particleLifeTimeS);
                vfx.SetInt("ParticuleCountCreation", particleCountPerTriggering);
            }

            // trigger event loop
            for (int ii = 0; ii < nbTriggerings; ++ii) {
                foreach (var cloud in dataVFX.cloudsGO) {
                    var vfx = cloud.GetComponent<VisualEffect>();
                    if (reduceCloudVisibiltyWhileGenerating) {
                        vfx.SetInt("Visibility", (int)(1000 - 1000 * (1f * ii / nbTriggerings)));
                    }
                    vfx.SendEvent(eventId);
                }
                yield return new WaitForSeconds(durationS / nbTriggerings);
            }

            // you can trigger any number of events
            // ...
        }
    }
}