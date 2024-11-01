

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

// unity
using UnityEngine;

namespace BS {


    public class VFXBaseProfile : MonoBehaviour {

        public GameObject dataVFXGO = null;
        protected IEnumerator profileEventsCo = null;
        protected DCDataVFX dataVFX = null;

        public bool applyInitDisplaySettings = true;
        public List<DCCloudDisplaySettings> initDisplaySettings = new List<DCCloudDisplaySettings>();

        public void start_events() {

            if (dataVFXGO == null) {
                dataVFXGO = transform.gameObject;
                UnityEngine.Debug.LogWarning("No gameobject defined, use current gameobject for retrieving DCDataVFX component.");
            }
            dataVFX = dataVFXGO.GetComponent<DCDataVFX>();

            if (dataVFX == null) {
                UnityEngine.Debug.LogError("No DCDataVFX component found. Cannot srart events.");
                return;
            }

            init_shader_values();
            StartCoroutine(profileEventsCo = profile_co());
        }

        protected virtual void init_shader_values() {

            if (dataVFX == null) {
                return;
            }

            for (int idC = 0; idC < dataVFX.clouds_count(); ++idC) {
                if (idC < initDisplaySettings.Count) {
                    dataVFX.set_tint(idC, initDisplaySettings[idC].tint);
                    dataVFX.set_display_state(idC, initDisplaySettings[idC].display);
                    dataVFX.set_update_state(idC, initDisplaySettings[idC].update);
                    dataVFX.set_color_factor(idC, initDisplaySettings[idC].colorFactor);
                    dataVFX.set_octogon_crop_factor(idC, initDisplaySettings[idC].octogonCropFactor);
                    dataVFX.set_particle_size(idC, initDisplaySettings[idC].particleSize);
                }
            }
        }

        protected virtual IEnumerator profile_co() {
            UnityEngine.Debug.Log("Default profile, override this function.");
            yield return new WaitForSeconds(0.1f);
        }
    }

}