
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

// unity
using UnityEngine;

namespace Ex {

    public class RuntimeFPPCamera : MonoBehaviour {

        public float speedT = 0.5f;
        public float speedRH = 3f;
        public float speedRV = 3f;
        public float speedRR = 3f;
        public bool outside = true;

        public float lastNx = 0f;
        public float lastNy = 0f;
        public bool lastOutsideState = false;

        Camera cameraC = null;
        private void Awake() {
            cameraC = GetComponent<Camera>();
        }

        public bool is_mouse_outside() {

            if (!cameraC.enabled) {
                return true;
            }

            var view = cameraC.ScreenToViewportPoint(UnityEngine.Input.mousePosition);
            return (view.x < 0 || view.x > 1 || view.y < 0 || view.y > 1);
        }

        private void Update() {

            bool leftClick = UnityEngine.Input.GetMouseButton(0);
            bool rightClick = UnityEngine.Input.GetMouseButton(1);
            bool middleClick = UnityEngine.Input.GetMouseButton(2);
            float nx = UnityEngine.Input.GetAxis("Mouse X");
            float ny = UnityEngine.Input.GetAxis("Mouse Y");
            float scroll = UnityEngine.Input.GetAxis("Mouse ScrollWheel");
            outside = is_mouse_outside();

            if (outside) {
                lastNx = nx;
                lastNy = ny;
                lastOutsideState = outside;
                return;
            }


            if (scroll > 0f) {
                move_forward(speedT);
            } else if (scroll < 0f) {
                move_backward(speedT);
            }

            if (!lastOutsideState) {

                if (middleClick) {

                    // check horizontal right click mouse drag movement
                    if (nx != 0) {
                        //Debug.Log("NX " + nx + " " + lastNx + " " + lastOutsideState + " " + outside);
                        horizontal_strafe(nx * speedT);
                    }
                    // check vertical right click mouse drag movement
                    if (ny != 0) {
                        //Debug.Log("NY " + ny + " " + lastNy);
                        vertical_strafe(ny * speedT);
                    }

                } else if (leftClick && rightClick) {
                    transform.Rotate(0, 0, speedRR * nx);
                } else if (rightClick) {
                    transform.Rotate(new Vector3(-ny * speedRV, nx * speedRH, 0));
                } else if (leftClick) {
                    // ...
                }
            }

            lastNx = nx;
            lastNy = ny;
            lastOutsideState = outside;
        }

        protected void horizontal_strafe(float amount) {
            transform.position = transform.position + transform.right * amount;
        }

        protected void vertical_strafe(float amount) {
            transform.position = transform.position + transform.up * amount;
        }

        protected void move_forward(float amount) {
            transform.position += transform.forward * amount;
        }

        protected void move_backward(float amount) {
            transform.position -= transform.forward * amount;
        }
    }
}


