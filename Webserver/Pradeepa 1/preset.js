let currentPreset = null;
        let presets = {
            "Preset 1": ["Move 100cm", "Turn right", "Move 50cm"],
            "Preset 2": ["Move 200cm", "Turn left"],
            "Preset 3": ["Move 300cm", "Turn right", "Move 100cm"],
            "Preset 4": ["Move 400cm", "Turn left", "Move 200cm"]
        };

        function displayPresets() {
            const presetList = document.getElementById('preset-list');
            presetList.innerHTML = ''; // Clear current list
            for (const presetName in presets) {
                const presetDiv = document.createElement('div');
                presetDiv.textContent = presetName;
                const editButton = document.createElement('button');
                editButton.className = 'button button-green';
                editButton.textContent = 'Edit';
                editButton.onclick = () => loadPreset(presetName);
                const deleteButton = document.createElement('button');
                deleteButton.className = 'button button-red';
                deleteButton.textContent = 'Delete';
                deleteButton.onclick = () => deletePreset(presetName);
                const buttonContainer = document.createElement('div');
                buttonContainer.className = 'preset-buttons';
                buttonContainer.appendChild(editButton);
                buttonContainer.appendChild(deleteButton);
                presetDiv.appendChild(buttonContainer);
                presetList.appendChild(presetDiv);
            }
        }

        function loadPreset(presetName) {
            currentPreset = presetName;
            const stepList = document.getElementById('step-list');
            stepList.innerHTML = ''; // Clear current steps
            presets[presetName].forEach(step => {
                const stepElement = document.createElement('div');
                stepElement.textContent = step;
                stepList.appendChild(stepElement);
            });
            alert('Loaded ' + presetName);
        }

        function addStep() {
            const stepList = document.getElementById('step-list');
            const step = document.createElement('div');
            step.textContent = 'New Step';
            stepList.appendChild(step);
        }

        function savePreset() {
            if (!currentPreset) {
                alert('No preset selected');
                return;
            }
            const steps = [];
            const stepList = document.getElementById('step-list').children;
            for (let step of stepList) {
                steps.push(step.textContent);
            }
            presets[currentPreset] = steps;
            alert('Preset saved');
            displayPresets();
        }

        function deletePreset(presetName) {
            delete presets[presetName];
            if (presetName === currentPreset) {
                currentPreset = null;
                document.getElementById('step-list').innerHTML = ''; // Clear step list
            }
            alert('Deleted ' + presetName);
            displayPresets();
        }

        function deleteCurrentPreset() {
            if (!currentPreset) {
                alert('No preset selected');
                return;
            }
            deletePreset(currentPreset);
        }

        function sendToESP32() {
            if (!currentPreset) {
                alert('No preset selected');
                return;
            }
            // Function to send output to ESP32 (to be implemented)
            alert('Sending data to ESP32 for ' + currentPreset);
        }

        // Initialize the preset list on page load
        window.onload = displayPresets;