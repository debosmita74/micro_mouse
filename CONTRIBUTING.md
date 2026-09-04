# Contributing to Micro Mouse

Thank you for considering contributing! Here's how you can help.

## 📋 How to Contribute

### 1. Reporting Bugs
- Use the [GitHub Issues](../../issues) tab
- Include: steps to reproduce, expected vs actual behavior, hardware/software versions
- Attach serial logs or sensor readings if relevant

### 2. Suggesting Features
- Open an issue with the `enhancement` label
- Describe the use case and expected behavior

### 3. Submitting Code

#### Fork & Branch
```bash
git clone https://github.com/YOUR_USERNAME/micro_mouse.git
cd micro_mouse
git checkout -b feature/your-feature-name
```

#### Branch Naming Convention
| Type | Format | Example |
|------|--------|---------|
| Feature | `feature/short-description` | `feature/add-diagonal-movement` |
| Bug Fix | `fix/short-description` | `fix/sensor-calibration` |
| Docs | `docs/short-description` | `docs/update-readme` |
| Hardware | `hw/short-description` | `hw/update-schematic-v2` |

#### Commit Message Convention
```
<type>(<scope>): <short summary>

<optional body>
```

**Types:** `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`, `hw`

**Examples:**
```
feat(solver): implement A* pathfinding algorithm
fix(pid): correct integral windup in low-speed turns
docs(readme): add circuit diagram and BOM table
hw(pcb): update motor driver footprint
```

#### Pull Request Process
1. Ensure code compiles without warnings (`-Wall -Wextra`)
2. Test on hardware if possible
3. Update documentation if needed
4. Submit a PR against `main` branch

## 🧹 Code Style
- Follow the project's formatting conventions
- Use `snake_case` for functions and variables
- Use `UPPER_CASE` for macros and constants
- Use `PascalCase` for typedef structs/enums
- Comment all public functions with Doxygen-style docs
- Max line length: 100 characters

## 📜 Code of Conduct
Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md).
