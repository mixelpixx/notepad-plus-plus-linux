# Contributing to Notepad++ Linux

Thank you for your interest in contributing to Notepad++ Linux! This document provides guidelines for contributing to the project.

## Getting Started

### Prerequisites
- Linux development environment (Ubuntu 20.04+ recommended)
- Qt6 or Qt5 development libraries
- CMake 3.16+
- GCC 9+ with C++17 support
- Git for version control

### Development Setup
1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/yourusername/notepad-plus-plus-linux.git
   cd notepad-plus-plus-linux
   ```
3. Install dependencies:
   ```bash
   sudo apt install qt6-base-dev libqscintilla2-qt6-dev cmake build-essential
   ```
4. Build the project:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   make -j$(nproc)
   ```

## Development Guidelines

### Code Style
- Follow Qt coding conventions
- Use 4 spaces for indentation (no tabs)
- Maximum line length of 100 characters
- Use camelCase for functions and variables
- Use PascalCase for class names
- Include appropriate header comments

### C++ Standards
- Use modern C++17 features where appropriate
- Prefer smart pointers over raw pointers
- Use const correctness throughout
- Follow RAII principles
- Minimize use of macros

### Qt-Specific Guidelines
- Use Qt naming conventions (m_ prefix for member variables)
- Prefer Qt containers over STL when interfacing with Qt APIs
- Use Qt's signal/slot mechanism properly
- Implement proper parent-child relationships for Qt objects

## Contributing Process

### Before You Start
- Check existing issues to avoid duplicate work
- Create an issue to discuss large changes
- Ensure your contribution aligns with project goals

### Making Changes
1. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
2. Make your changes following the coding guidelines
3. Test your changes thoroughly
4. Commit with clear, descriptive messages
5. Push to your fork and create a pull request

### Pull Request Guidelines
- Provide a clear description of changes
- Include test cases if applicable
- Ensure all tests pass
- Update documentation as needed
- Keep pull requests focused and atomic

## Types of Contributions

### Bug Fixes
- Include steps to reproduce the bug
- Provide a minimal test case if possible
- Ensure the fix doesn't break existing functionality

### New Features
- Discuss the feature in an issue first
- Ensure the feature fits the project's scope
- Include comprehensive tests
- Update documentation

### Documentation
- Fix typos and improve clarity
- Add examples where helpful
- Keep documentation up to date with code changes

### Testing
- Add unit tests for new functionality
- Improve test coverage
- Report and fix test failures

## Code Review Process

### Review Criteria
- Code follows style guidelines
- Changes are well-tested
- Documentation is updated
- Performance impact is considered
- Security implications are evaluated

### Review Timeline
- Initial review within 1 week
- Feedback addressed promptly
- Final approval before merge

## Reporting Issues

### Bug Reports
Include:
- Operating system and version
- Qt version
- Steps to reproduce
- Expected vs actual behavior
- Error messages or logs
- Screenshots if applicable

### Feature Requests
Include:
- Clear description of the feature
- Use cases and benefits
- Implementation suggestions
- Willingness to implement

## Development Environment

### Recommended Tools
- Qt Creator or Visual Studio Code
- Git with proper configuration
- Valgrind for memory checking
- GDB for debugging
- Clang-format for code formatting

### Testing
- Manual testing with various file types
- Performance testing with large files
- Memory leak testing with Valgrind
- Cross-platform testing when possible

## Project Architecture

### Key Components
- `core/`: Main application logic
- `ui/`: User interface components
- `utils/`: Utility classes and helpers
- `platform/`: Platform-specific code

### Design Principles
- Separation of concerns
- Loose coupling between components
- Testable code structure
- Platform abstraction where needed

## Communication

### Channels
- GitHub Issues for bug reports and feature requests
- GitHub Discussions for general questions
- Pull request comments for code review

### Guidelines
- Be respectful and constructive
- Search existing issues before creating new ones
- Provide detailed information
- Follow up on your contributions

## Release Process

### Version Numbering
- Follow semantic versioning (MAJOR.MINOR.PATCH)
- Major: Breaking changes
- Minor: New features, backwards compatible
- Patch: Bug fixes

### Release Checklist
- All tests pass
- Documentation updated
- Changelog updated
- Version numbers bumped
- Package builds successfully

## License

By contributing to Notepad++ Linux, you agree that your contributions will be licensed under the MIT License.

## Recognition

Contributors will be recognized in:
- CHANGELOG.md for notable contributions
- README.md acknowledgments section
- Git commit history

Thank you for contributing to Notepad++ Linux!