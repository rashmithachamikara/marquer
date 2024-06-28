document.addEventListener("DOMContentLoaded", () => {
    const creatorPage = document.getElementById('creator-page');
    const presetsPage = document.getElementById('presets-page');
    const stepList = document.getElementById('step-list');
    const presetsList = document.getElementById('presets-list');
    const canvas = document.getElementById('route-canvas');
    const ctx = canvas.getContext('2d');

    const drawRoute = (steps) => {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        let x = canvas.width / 2;
        let y = canvas.height / 2;

        ctx.beginPath();
        ctx.moveTo(x, y);

        steps.forEach((step, index) => {
            const [direction, distance] = step.split(' ');
            const dist = parseInt(distance);
            switch (direction) {
                case 'left':
                    x -= dist;
                    break;
                case 'right':
                    x += dist;
                    break;
                case 'up':
                    y -= dist;
                    break;
                case 'down':
                    y += dist;
                    break;
                case '0°':
                    x += dist;
                    break;
                case '90°':
                    y -= dist;
                    break;
                case '180°':
                    x -= dist;
                    break;
                case '270°':
                    y += dist;
                    break;
                default:
                    break;
            }
            ctx.lineTo(x, y);
        });

        ctx.stroke();
        
        // Draw start point
        ctx.beginPath();
        ctx.arc(canvas.width / 2, canvas.height / 2, 5, 0, 2 * Math.PI);
        ctx.fillStyle = 'green';
        ctx.fill();

        // Draw end point
        ctx.beginPath();
        ctx.arc(x, y, 5, 0, 2 * Math.PI);
        ctx.fillStyle = 'red';
        ctx.fill();
    };

    const updateStepList = () => {
        const steps = Array.from(stepList.children).map(step => step.dataset.step);
        drawRoute(steps);
    };

    const createStepElement = (step) => {
        const li = document.createElement('li');
        li.classList.add('step-item');
        li.dataset.step = step;

        const stepText = document.createElement('span');
        stepText.textContent = step;

        const stepControls = document.createElement('div');
        stepControls.classList.add('step-controls');

        const upButton = document.createElement('button');
        upButton.textContent = '↑';
        upButton.addEventListener('click', () => {
            const prev = li.previousElementSibling;
            if (prev) {
                stepList.insertBefore(li, prev);
                updateStepList();
            }
        });

        const downButton = document.createElement('button');
        downButton.textContent = '↓';
        downButton.addEventListener('click', () => {
            const next = li.nextElementSibling;
            if (next) {
                stepList.insertBefore(next, li);
                updateStepList();
            }
        });

        const removeButton = document.createElement('button');
        removeButton.textContent = 'X';
        removeButton.addEventListener('click', () => {
            li.remove();
            updateStepList();
        });

        stepControls.appendChild(upButton);
        stepControls.appendChild(downButton);
        stepControls.appendChild(removeButton);

        li.appendChild(stepText);
        li.appendChild(stepControls);

        return li;
    };

    document.getElementById('view-presets').addEventListener('click', () => {
        creatorPage.classList.add('hidden');
        presetsPage.classList.remove('hidden');
    });

    document.getElementById('back-to-creator').addEventListener('click', () => {
        presetsPage.classList.add('hidden');
        creatorPage.classList.remove('hidden');
    });

    document.getElementById('add-step').addEventListener('click', () => {
        const direction = document.getElementById('direction').value;
        const distance = document.getElementById('distance').value;

        if ((direction && distance) || (direction && direction.endsWith('°'))) {
            const step = `${direction} ${distance}cm`;
            const li = createStepElement(step);
            stepList.appendChild(li);
            document.getElementById('direction').value = '';
            document.getElementById('distance').value = '';

            updateStepList();
        }
    });

    document.getElementById('save').addEventListener('click', () => {
        const presetName = document.getElementById('preset-name').value;
        const steps = Array.from(stepList.children).map(step => step.dataset.step);

        if (presetName && steps.length > 0) {
            const preset = document.createElement('div');
            const presetButton = document.createElement('button');
            presetButton.textContent = presetName;
            presetButton.addEventListener('click', () => {
                document.getElementById('preset-name').value = presetName;
                stepList.innerHTML = '';
                steps.forEach(step => {
                    const li = createStepElement(step);
                    stepList.appendChild(li);
                });
                presetsPage.classList.add('hidden');
                creatorPage.classList.remove('hidden');
                drawRoute(steps);
            });

            const selectButton = document.createElement('button');
            selectButton.textContent = 'Select';
            selectButton.addEventListener('click', () => {
                document.getElementById('preset-name').value = presetName;
                stepList.innerHTML = '';
                steps.forEach(step => {
                    const li = createStepElement(step);
                    stepList.appendChild(li);
                });
                presetsPage.classList.add('hidden');
                creatorPage.classList.remove('hidden');
                drawRoute(steps);
            });

            preset.appendChild(presetButton);
            preset.appendChild(selectButton);
            presetsList.appendChild(preset);
        }
    });
});
