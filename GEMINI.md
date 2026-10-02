# Project Rules & Development Workflow

## Version Control & GitHub Synchronization
- **Mandatory GitHub Push**: All code changes, bug fixes, features, optimizations, and documentation updates must **ALWAYS** be committed and pushed to GitHub (`origin main`) after verification.
- **Security & Credential Masking**: Never commit or push API keys, private passwords, server IP addresses, or tokens to GitHub.

## Server Deployment
- **Daemon Lifecycle**: Whenever C++ code changes are compiled and verified, existing server daemons (`loginserver`, `charserver`, `worldserver`) must be cleanly restarted.
