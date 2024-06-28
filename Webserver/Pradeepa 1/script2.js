function loadPreset(presetName) {
    // Function to load a preset
    alert('Loading ' + presetName);
}

function addStep() {
    // Function to add a step
    const stepList = document.getElementById('step-list');
    const step = document.createElement('div');
    step.textContent = 'New Step';
    stepList.appendChild(step);
}

function sendToESP32() {
    // Function to send output to ESP32
    alert('Sending data to ESP32');
}
document.addEventListener("DOMContentLoaded", function () {
    const directionInput = document.getElementById("direction");
    const distanceInput = document.getElementById("distance");
    const addStepButton = document.getElementById("add-step");
    const stepsList = document.getElementById("steps");
    const previewCanvas = document.getElementById("previewCanvas");
    const savePresetButton = document.getElementById("save-preset");
    const presetsList = document.getElementById("presets-list");
    const ctx = previewCanvas.getContext("2d");

    let steps = [];
    let presets = [];

    function drawRoute() {
        ctx.clearRect(0, 0, previewCanvas.width, previewCanvas.height);
        ctx.strokeStyle = "gray";
        ctx.lineWidth = 4;
        ctx.beginPath();

        let x = previewCanvas.width / 2;
        let y = previewCanvas.height / 2;
        ctx.moveTo(x, y);

        steps.forEach(step => {
            if (step.direction.toLowerCase() === "move") {
                let distance = parseInt(step.distance, 10);
                ctx.lineTo(x, y -= distance);
            } else if (step.direction.toLowerCase() === "turn right") {
                ctx.moveTo(x += 50, y);
            }
        });

        ctx.stroke();

        ctx.beginPath();
        ctx.arc(x, y, 10, 0, 2 * Math.PI);
        ctx.fillStyle = "green";
        ctx.fill();
    }

    function renderSteps() {
        stepsList.innerHTML = "";
        steps.forEach(step => {
            const listItem = document.createElement("li");
            listItem.textContent = step.direction ? step.direction : `Move ${step.distance}cm`;
            stepsList.appendChild(listItem);
        });
        drawRoute();
    }

    function renderPresets() {
        presetsList.innerHTML = "";
        presets.forEach((preset, index) => {
            const listItem = document.createElement("li");
            listItem.textContent = preset.name;
            listItem.addEventListener("click", () => {
                steps = preset.steps;
                renderSteps();
            });
            const editButton = document.createElement("button");
            editButton.textContent = "Edit";
            editButton.addEventListener("click", (event) => {
                event.stopPropagation();
                // Handle preset editing logic
            });
            const deleteButton = document.createElement("button");
            deleteButton.textContent = "Delete";
            deleteButton.addEventListener("click", (event) => {
                event.stopPropagation();
                presets.splice(index, 1);
                renderPresets();
            });
            listItem.appendChild(editButton);
            listItem.appendChild(deleteButton);
            presetsList.appendChild(listItem);
        });
    }

    addStepButton.addEventListener("click", function () {
        const direction = directionInput.value.trim();
        const distance = distanceInput.value.trim();

        if ((direction && !distance) || (!direction && distance)) {
            const step = { direction, distance };
            steps.push(step);
            directionInput.value = "";
            distanceInput.value = "";
            renderSteps();
        } else {
            alert("Only one of these fields can be filled at a time");
        }
    });

    savePresetButton.addEventListener("click", function () {
        const presetName = prompt("Enter preset name:");
        if (presetName) {
            const preset = { name: presetName, steps: [...steps] };
            presets.push(preset);
            renderPresets();
        }
    });
});
