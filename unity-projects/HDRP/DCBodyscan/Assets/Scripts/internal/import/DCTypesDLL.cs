
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
using Unity.Collections;

namespace BS {

    [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
    public struct DLLVertex {
        public Vector3 pos;
        public Color32 col;
    }

    public struct DLLVec3 {
        public Vector3 pos;
    }

    public struct DLLVec4 {
        public Vector4 pos;
    }

    public class NativeDLLVec3 {

        public DLLVec3[] data = null;
        public NativeArray<DLLVec3> native;

        public NativeDLLVec3(int nbVertices) {
            data    = new DLLVec3[nbVertices];
            native  = new NativeArray<DLLVec3>(data, Allocator.Persistent);
        }

        public void clean() {
            native.Dispose();
        }
    }


    public class NativeDLLVec4{

        public DLLVec4[] data = null;
        public NativeArray<DLLVec4> native;

        public NativeDLLVec4(int nbVertices) {
            data = new DLLVec4[nbVertices];
            native = new NativeArray<DLLVec4>(data, Allocator.Persistent);
        }

        public void clean() {
            native.Dispose();
        }
    }
}