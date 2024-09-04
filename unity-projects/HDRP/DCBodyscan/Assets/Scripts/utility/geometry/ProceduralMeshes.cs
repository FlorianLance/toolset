

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

namespace BS {

    public class ProceduralMeshes {

        public static Mesh low_poly_paraboloid(float size = 0.5f, float depth = 0.4f) {

            Mesh mesh = new Mesh();

            float s = size;
            float d = depth;
            Vector3[] vertices = new Vector3[8] {
                new Vector3(-s,  -s,    d),
                new Vector3(0f,  -s,    0f),
                new Vector3(s,   -s,    d),
                new Vector3(-s,   0f,   0f),
                new Vector3(s,    0f,   0f),
                new Vector3(-s,   s,    d),
                new Vector3(0f,   s,    0f),
                new Vector3(s,    s,    d)
            };

            int[] triangles = new int[6 * 3] {
                0,3,1, 1,4,2, 1,3,4, 3,5,6, 3,6,4, 4,6,7
            };

            mesh.vertices = vertices;
            mesh.triangles = triangles;

            return mesh;
        }

        public static Mesh paraboloid(float outerSize = 0.25f, float innerSier = 0.5f, float depth = 0.4f) {

            Mesh mesh = new Mesh();

            float oS = outerSize;
            float iS = innerSier;
            float d = depth;

            float hIs = iS * 0.5f;

            Vector3[] vertices = new Vector3[12] {
                new Vector3(-hIs,          -oS-hIs,    d),
                new Vector3(hIs,           -oS-hIs,    d),
                new Vector3(-oS-hIs,       -hIs,       d),
                new Vector3(-hIs,          -hIs,       0f),
                new Vector3(hIs,           -hIs,       0f),
                new Vector3(hIs + oS,      -hIs,       d),
                new Vector3(-oS-hIs,       hIs,        d),
                new Vector3(-hIs,          hIs,        0f),
                new Vector3(hIs,           hIs,        0f),
                new Vector3(hIs + oS,      hIs,        d),
                new Vector3(-hIs,          hIs + oS,   d),
                new Vector3(hIs,           hIs + oS,   d)
            };

            int[] triangles = new int[14 * 3] {
                0,2,3,  0,3,1,  1,3,4,   1,4,5, 2,6,3,
                3,6,7,  3,7,4,  4,7,8,   5,4,8, 5,8,9,
                6,10,7, 7,10,8, 8,10,11, 8,11,9
            };

            mesh.vertices = vertices;
            mesh.triangles = triangles;

            return mesh;
        }
    }
}