
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
using System.IO;
using UnityEditor;


// unity
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace BS {

#if UNITY_EDITOR

    [CustomEditor(typeof(ApplyTransformFromJSON))]
    public class ApplyTransformFromJSONEditor : Editor {

        private ApplyTransformFromJSON applyTranformFromJSON { get { return target as ApplyTransformFromJSON; } }

        public override bool RequiresConstantRepaint() {
            return true;
        }

        public override void OnInspectorGUI() {

            //GUILayout.TextArea("Type the base path for the JSON file (id of the grabber and JSON ext will be added automatically");
            if (GUILayout.Button("Save transform to JSON file (using current defined path")) {
                try {
                    TransformValue tv = new TransformValue();
                    tv.position = applyTranformFromJSON.transform.localPosition;
                    tv.rotation = applyTranformFromJSON.transform.localRotation;
                    tv.scale    = applyTranformFromJSON.transform.localScale;
                    File.WriteAllText(applyTranformFromJSON.transformmJSONFilePath, JsonUtility.ToJson(tv));
                } catch (System.Exception ex) {
                    UnityEngine.Debug.LogError(string.Format("[DCDataVFXEditor::OnInspectorGUI] Cannot save JSON settings, error: [{0}] ", ex.Message));
                }
            }

            EditorGUILayout.Separator();
            base.OnInspectorGUI();
        }
    }
#endif

    public class ApplyTransformFromJSON : MonoBehaviour {

        public string transformmJSONFilePath = "./transform.json";

        // Start is called before the first frame update
        void Start() {

            try {
                string jsonContent = File.ReadAllText(transformmJSONFilePath);
                var newTransform = JsonUtility.FromJson<TransformValue>(jsonContent);
                if (newTransform != null) {
                    transform.localPosition = newTransform.position;
                    transform.localRotation = newTransform.rotation;
                    transform.localScale    = newTransform.scale;
                    UnityEngine.Debug.Log(string.Format("Transform loaded from JSON file with path [{0}] with text [{1}]and applied to GameObject [{2}]", transformmJSONFilePath, jsonContent, transform.name));
                } else {
                    UnityEngine.Debug.LogError(string.Format("[ApplyTransformFromJSON::Awake] Cannot convert text to JSON from file with path [{0}] with text: [{1}] ", transformmJSONFilePath, jsonContent));
                }

            } catch (System.Exception ex) {
                UnityEngine.Debug.LogWarning(string.Format("[ApplyTransformFromJSON::Awake] Cannot read file with path [{0}], error: [{1}] ", transformmJSONFilePath, ex.Message));
            }
        }
    }
}