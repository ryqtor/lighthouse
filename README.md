# 🦙 Lighthouse: High-Performance C++ Terminal Client & RAG Engine for llama.cpp

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Docker](https://img.shields.io/badge/docker-ready-blueviolet.svg)](Dockerfile)

**Lighthouse** is a lightweight, ultra-fast, and modular C++ terminal client, REST server middleware, and Retrieval-Augmented Generation (RAG) framework designed to interface directly with `llama.cpp` server backends.

Built for performance and flexibility, Lighthouse combines an interactive CLI terminal experience with full-featured session management, SQLite persistence, SSE token streaming, JWT authentication, and vector embeddings for local context-aware AI applications.

---

## ✨ Key Features

- 💻 **Interactive Terminal CLI**: Rich interactive console with real-time token streaming, colorized multi-actor roleplay support, and live thought processing display (`pthink`).
- ⚡ **REST API & SSE Streaming**: Server-Sent Events (SSE) streaming client and OpenAI-compatible endpoint adapters for low-latency token generation.
- 🔐 **Auth & Middleware Pipeline**: Token handling, custom header processing, and JWT validation middleware for secure multi-tenant deployments.
- 💾 **SQLite Session & History Persistence**: Persistent session state, conversational chat history management, automated schema migration, and adaptive context trimming.
- 🧠 **Vector Search & Local RAG**: Document ingestion pipeline, paragraph chunking, vector indexing, similarity search, and dynamic prompt assembly.
- 📊 **Metrics & Monitoring**: Real-time throughput (tokens/sec), generation latency tracking, system memory monitoring, and automated logging.
- 🌐 **Web Dashboard**: Modern web interface (`/web`) for monitoring server status, managing sessions, and testing completions.
- 🐳 **Production Docker & CI/CD**: Ready-to-use `Dockerfile` and `docker-compose.yml` with automated GitHub Actions workflow for static cross-compilation.

---

## 🏗️ Architecture Overview

```
 ┌────────────────────────────────────────────────────────┐
 │            Clients: Terminal CLI / Web Dashboard       │
 └──────────────────────────┬─────────────────────────────┘
                            │
 ┌──────────────────────────▼─────────────────────────────┐
 │               Lighthouse Middleware Layer              │
 │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
 │  │ JWT Auth     │  │ Stream Guard │  │ SSE Client   │  │
 │  └──────────────┘  └──────────────┘  └──────────────┘  │
 └──────────────────────────┬─────────────────────────────┘
                            │
 ┌──────────────────────────▼─────────────────────────────┐
 │                Core State & Local RAG                  │
 │  ┌──────────────────────┐   ┌──────────────────────┐   │
 │  │ SQLite Persistence   │   │ Vector Embeddings    │   │
 │  │ & Session Manager    │   │ & Chunk Ingestion    │   │
 │  └──────────────────────┘   └──────────────────────┘   │
 └──────────────────────────┬─────────────────────────────┘
                            │ (HTTP / SSE / REST)
 ┌──────────────────────────▼─────────────────────────────┐
 │               llama.cpp Server Backend                 │
 └────────────────────────────────────────────────────────┘
```

---

## 🚀 Getting Started

### Prerequisites

- **GCC / G++**: Supporting C++17 (`g++ 9.4+` on Linux or MinGW `g++ 14+` on Windows)
- **Make**: Standard POSIX make or MinGW make
- **llama.cpp server**: A running instance of `llama.cpp` server (default endpoint: `http://127.0.0.1:8080`)

### Building from Source

Clone the repository and initialize submodules:

```bash
git clone https://github.com/ryqtor/lighthouse.git
cd lighthouse
git submodule init
git submodule update
```

#### Build Standard Executable
```bash
make chat
```

#### Build Static Binary (Recommended for production)
```bash
make static
```

The compiled executable and default runtime configs will be output to the `dist/` directory.

---

## 💻 Usage & Executable Options

Run the binary from the project root or `dist/` directory:

```bash
./dist/chat --prompt default --ip 127.0.0.1 --port 8080
```

### Command-Line Arguments

| Flag | Description | Default |
| :--- | :--- | :--- |
| `--prompt <name>` | Select prompt profile from `prompts.json` | `default` |
| `--param-profile <name>` | Select parameter profile from `params.json` | `default` |
| `--chat-template <name>` | Select chat formatting template from `templates.json` | `None` |
| `--ip <address>` | Server IP address | `127.0.0.1` |
| `--port <port>` | Server port number | `8080` |
| `--no-chat-tags` | Disable actor tags (e.g. `User:`, `Assistant:`) | `false` |
| `--no-chat-guards` | Disable stream guard token stopping | `false` |
| `--debug` | Enable verbose logging into `lighthouse.log` | `false` |

---

## 📝 Interactive Terminal Slash Commands

Inside the terminal shell, prefix commands with `/`:

### 🗣️ Conversation & Roleplay
- `/narrator` — Trigger a narration completion from the narrator persona.
- `/actor <name>` or `/now <name>` — Set or create the active speaking persona (e.g. `/now Einstein`).
- `/as <name>` — Prompt as a specific character persona (e.g. `/as Einstein`).
- `/talkto <character>` — Direct the dialogue to a specific character.
- `/insert` or `/i` — Enter multiline input mode. Type `EOL` on a new line or specify a file path to insert content.
- `/retry` or `/r` — Regenerate the last assistant completion.
- `/continue` — Resume generation without additional user input.
- `/edit` — Edit the last assistant message.
- `/undo` or `/u` — Revert the last user message and assistant turn.
- `/undolast` — Revert only the last assistant completion.

### ⚙️ Engine Settings & Views
- `/chat on|off` — Toggle displaying conversation actor tags.
- `/pthink on|off` — Show or hide internal reasoning thoughts during generation.
- `/lprompt` — Print the formatted prompt sent to the LLM backend.
- `/lactors` — List active conversation actors.
- `/lparams` — Display active inference parameters.
- `/rparams` — Reload active parameter profile from disk.
- `/rtemplate` — Reload active template profile from disk.
- `/sparam <name>` — Switch active parameter profile at runtime.
- `/stemplate <name>` — Switch active template profile at runtime.
- `/sprompt <name>` — Switch prompt configuration profile at runtime.
- `/ssystem <prompt>` — Set a new root system prompt.
- `/save <session_name>` — Save active session to SQLite persistence.
- `/load <session_name>` — Load an existing session state.
- `/redraw` — Redraw active conversation screen.
- `/reset` — Reset session state and clear message history.
- `/help` — Show command reference guide.
- `/quit` or `/q` — Exit the application.

---

## ⚙️ Configuration Files

Lighthouse uses JSON configuration files located in `config/`:

- **`config/prompts.json`**: Defs for system prompts, character actors, and prompt behavior.
- **`config/params.json`**: Inference sampling settings (`temperature`, `top_p`, `top_k`, `penalty_repeat`, `max_tokens`).
- **`config/templates.json`**: Pre-formatted chat templates (ChatML, Llama-3, Mistral, Alpaca, etc.).

---

## 🐳 Running with Docker

### Using Docker Compose

```bash
docker-compose up -d
```

### Manual Docker Build & Run

```bash
docker build -t lighthouse:latest .
docker run -it --network="host" lighthouse:latest
```

---

## 🧪 Development & Testing

Run local build verification:

```bash
make clean && make static
```

CI workflows are configured under `.github/workflows/ci.yml` for continuous integration on both Windows and Linux toolchains.

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
