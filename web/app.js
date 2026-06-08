// Lighthouse Dashboard App
(function() {
    const API_BASE = 'http://127.0.0.1:8080';

    const elements = {
        statusBadge: document.getElementById('statusBadge'),
        sessionList: document.getElementById('sessionList'),
        newSessionBtn: document.getElementById('newSessionBtn'),
        chatMessages: document.getElementById('chatMessages'),
        chatInput: document.getElementById('chatInput'),
        sendBtn: document.getElementById('sendBtn'),
        tokensPerSec: document.getElementById('tokensPerSec'),
        memoryUsage: document.getElementById('memoryUsage'),
        uptimeDisplay: document.getElementById('uptimeDisplay')
    };

    let currentSession = null;
    const sessions = [];

    function updateStatus(connected) {
        elements.statusBadge.textContent = connected ? 'Connected' : 'Disconnected';
        elements.statusBadge.className = 'status-badge' + (connected ? ' connected' : '');
    }

    function addMessage(role, content) {
        const div = document.createElement('div');
        div.className = 'message message-' + role;
        div.style.cssText = `
            padding: 10px 14px;
            margin-bottom: 8px;
            border-radius: 8px;
            background: ${role === 'user' ? '#2e3348' : '#1a2340'};
            white-space: pre-wrap;
        `;
        div.textContent = (role === 'user' ? 'You: ' : 'Assistant: ') + content;
        elements.chatMessages.appendChild(div);
        elements.chatMessages.scrollTop = elements.chatMessages.scrollHeight;
    }

    function createSession() {
        const id = 'session_' + Date.now();
        sessions.push({ id, name: 'Chat ' + sessions.length });
        renderSessions();
        currentSession = id;
    }

    function renderSessions() {
        elements.sessionList.innerHTML = '';
        sessions.forEach(s => {
            const li = document.createElement('li');
            li.textContent = s.name;
            li.className = (s.id === currentSession) ? 'active' : '';
            li.onclick = () => { currentSession = s.id; renderSessions(); };
            elements.sessionList.appendChild(li);
        });
    }

    elements.newSessionBtn.addEventListener('click', createSession);

    elements.sendBtn.addEventListener('click', () => {
        const msg = elements.chatInput.value.trim();
        if (!msg) return;
        addMessage('user', msg);
        elements.chatInput.value = '';
    });

    elements.chatInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter' && !e.shiftKey) {
            e.preventDefault();
            elements.sendBtn.click();
        }
    });

    // Init
    updateStatus(false);
    createSession();
})();
