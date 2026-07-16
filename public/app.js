const output = document.getElementById('responseOutput');

document.getElementById('getUsersBtn').addEventListener('click', async () => {
    try {
        output.textContent = "Отправка запроса...";
        const res = await fetch('/api/users');
        const data = await res.json();
        output.textContent = JSON.stringify(data, null, 2);
    } catch (err) {
        output.textContent = "Ошибка: " + err.message;
    }
});
document.getElementById('createUserBtn').addEventListener('click', async () => {
    const nameInput = document.getElementById('usernameInput');
    if (!nameInput.value) {
        alert("Введите имя!");
        return;
    }

    try {
        output.textContent = "Отправка данных...";
        const res = await fetch('/api/users', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name: nameInput.value })
        });
        const data = await res.json();
        output.textContent = `Статус: ${res.status}\n` + JSON.stringify(data, null, 2);
        nameInput.value = '';
    } catch (err) {
        output.textContent = "Ошибка: " + err.message;
    }
});