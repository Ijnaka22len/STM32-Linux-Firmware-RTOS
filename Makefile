# Git Routines
# Default branch
BRANCH = main

.PHONY: gitfetch gitpull gitpush gitstash-sync help

# Fetch and Observe Changes
gitfetch:
	@echo "=== Fetching latest changes from GitHub ==="
	git fetch origin
	@echo ""
	@echo "=== Incoming commits from GitHub (not yet local) ==="
	-git log HEAD..origin/$(BRANCH) --oneline
	@echo ""
	@echo "=== Local status & potential conflicts ==="
	git status

# Pull down web changes
gitpull:
	@echo "=== Pulling changes from GitHub ==="
	git pull origin $(BRANCH)

# Commit local changes with a custom message and push
# Usage: make gitpush m="commit message"
gitpush:
	@if [ -z "$(m)" ]; then \
		echo "Error: You must provide a commit message. Example: make gitpush m='my commit message'"; \
		exit 1; \
	fi
	@echo "=== Staging, committing, and pushing changes ==="
	git add .
	git commit -m "$(m)"
	git push origin $(BRANCH)

# Configuration: Set Git Email
gitemail:
	@if [ -z "$(e)" ]; then \
		echo "Error: You must provide user email without extension @gmail.com. E.g e='example' for example@gmail.com"; \
		exit 1; \
	fi
	@echo "=== Configuring User email($(e)@gmail.com) ==="
	git config user.email "$(e)@gmail.com"

# Configuration: Set Git Name
gitname:
	@if [ -z "$(n)" ]; then \
		echo "Error: You must provide user name n='username'"; \
		exit 1; \
	fi
	@echo "=== Configuring User Name($(n)) ==="
	git config user.name "$(n)"

# Sync when BOTH sides have changes (Stash -> Pull -> Pop)
gitstash:
	@echo "=== Stashing local changes ==="
	-git stash
	@echo "=== Pulling web changes ==="
	git pull origin $(BRANCH)
	@echo "=== Re-applying your local changes ==="
	-git stash pop

# Helper menu
help:
	@echo "Available Git routines:"
	@echo "  make gitfetch          - Safe check: fetch and display differences"
	@echo "  make gitpull           - Scenario A: Pull changes from GitHub web"
	@echo "  make gitpush m='msg'   - Scenario B: Commit local work and push to GitHub"
	@echo "  make gitstash          - Scenario C: Sync both sides cleanly using stash"
