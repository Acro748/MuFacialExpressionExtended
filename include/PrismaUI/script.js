let currentData = [];
let activeCategoryIndex = 0;

window.UpdateMorphData = function (jsonString) {
    try {
        currentData = JSON.parse(jsonString);
        updateCategoryHeader();
        renderItems(activeCategoryIndex);
    } catch (e) {
        console.error("JSON Parse Error:", e);
    }
};

function updateCategoryHeader() {
    const categoryNameEl = document.getElementById('current-category-name');
    if (currentData && currentData[activeCategoryIndex]) {
        categoryNameEl.textContent = currentData[activeCategoryIndex].category;
    }
}

function renderItems(index) {
    const listElement = document.getElementById('data-list');
    listElement.innerHTML = '';

    if (!currentData[index] || !currentData[index].items) return;

    const items = currentData[index].items;
    const fragment = document.createDocumentFragment();

    for (const item of items) {
        const li = document.createElement('li');
        li.className = 'data-item';

        const nameSpan = document.createElement('span');
        nameSpan.className = 'data-name';
        nameSpan.textContent = item.name;

        const valueSpan = document.createElement('span');
        valueSpan.className = 'data-value';
        valueSpan.textContent = item.value;

        li.append(nameSpan, valueSpan);
        fragment.append(li);
    }

    listElement.append(fragment);
}

window.SwitchNextCategory = function () {
    if (!currentData || currentData.length === 0) return;

    activeCategoryIndex = (activeCategoryIndex + 1) % currentData.length;
    updateCategoryHeader();
    renderItems(activeCategoryIndex);
    const listElement = document.getElementById('data-list');
    if (listElement) listElement.scrollTop = 0;
};

window.SwitchPrevCategory = function () {
    if (!currentData || currentData.length === 0) return;

    activeCategoryIndex = (activeCategoryIndex - 1 + currentData.length) % currentData.length;
    updateCategoryHeader();
    renderItems(activeCategoryIndex);

    const listElement = document.getElementById('data-list');
    if (listElement) listElement.scrollTop = 0;
};

window.ScrollDataList = function (deltaString) {
    const delta = parseInt(deltaString, 10);
    if (isNaN(delta)) return;

    const listElement = document.getElementById('data-list');
    if (listElement) {
        listElement.scrollBy({ top: delta, behavior: 'smooth' });
    }
};

window.UpdateCateogryHintLeftText = function (keyName) {
    const hintElement = document.getElementById('category-hint-left');
    if (hintElement) {
        hintElement.textContent = keyName;
    }
};
window.UpdateCateogryHintRightText = function (keyName) {
    const hintElement = document.getElementById('category-hint-right');
    if (hintElement) {
        hintElement.textContent = keyName;
    }
};

window.UpdateUIPosition = function (jsonString) {
    try {
        const pos = JSON.parse(jsonString);
        document.documentElement.style.setProperty('--ui-pos-x', pos.x + '%');
        document.documentElement.style.setProperty('--ui-pos-y', pos.y + '%');
    } catch (e) {
        console.error("Position Parse Error:", e);
    }
};

window.UpdateUIScale = function (scaleString) {
    const scale = parseFloat(scaleString);
    if (!isNaN(scale)) {
        document.documentElement.style.setProperty('--ui-scale', scale);
    }
};
