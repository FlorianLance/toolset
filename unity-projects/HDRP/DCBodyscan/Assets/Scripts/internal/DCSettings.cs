
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
using System;
using System.Collections.Generic;

// unity
using UnityEngine;

namespace BS {

    [Serializable]
    public class TransformValue {
        public TransformValue() { }
        public TransformValue(Vector3 position, Quaternion rotation, Vector3 scale) {
            this.position = position;
            this.rotation = rotation;
            this.scale    = scale;
        }
        public Vector3 position = Vector3.zero;
        public Quaternion rotation = Quaternion.identity;
        public Vector3 scale = Vector3.one;
    }

    [Serializable]
    public class DCCloudDisplaySettings {
        public bool display = true;                     // shall the cloud be displayed ?
        public bool update = true;                      // shall the cloud be updated ?
        public bool enableBackFaceCulling = false;
        public Color tint = new Color(1f, 1f, 1f, 1f);  // tint color to be mixed with input cloud color (VFX)
        public float colorFactor = 3.0f;                // color multiplier factor applied at the end (VFX)
        public float particleSize = 0.005f;             // particle size (VFX)
        public float octogonCropFactor = 0.125f;        // octogon output mesh crop factor (VFX)                
    }

    [Serializable]
    public class DCDataVFXSettings {
        public bool applyParentTransform = false;
        public TransformValue parentTransform = new TransformValue();
        public List<DCCloudDisplaySettings> cloudsDisplayS = new List<DCCloudDisplaySettings>();
    }
}