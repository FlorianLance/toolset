
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
using System.Diagnostics;
using System;

namespace BS {


    [Serializable]
    public class DCVideoStates {
        public int deviceId = 0;
        public int currentFrameId = 0;
        public int currentFrameSize = 0;
        public int verticesCountToCopy = 0;
        public float[] deviceTransform = new float[16];
    }

    [Serializable]
    public class DCStreamingStates {
        public int deviceId = 0;
        public bool connected = false;
        public int currentFrameSize = 0;
        public int currentId = 0;
        public int verticesCountToCopy = 0;
        public int cloudUpdated = 0;
        public int averageCloudUpdatedPerSecond = 0;
        public Stopwatch sw = new Stopwatch();
    }
}