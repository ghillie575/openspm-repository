# openspm-repository

A command-line tool for managing OpenSPM package repositories. This tool helps you initialize repositories, manage package indexes, and handle package metadata for OpenSPM package manager.

## Features

- **Initialize repositories** with custom metadata
- **Rebuild package indexes** by scanning `.tar.gz` archives and extracting package information
- **Add external repository URLs** as dependencies
- **Extract package metadata** from archives automatically
- Parse and generate YAML-based package and repository metadata

## Table of Contents

- [Installation](#installation)
- [Building from Source](#building-from-source)
- [Usage](#usage)
  - [Initialize a Repository](#initialize-a-repository)
  - [Rebuild Package Index](#rebuild-package-index)
  - [Add External Repository URL](#add-external-repository-url)
- [Repository Structure](#repository-structure)
- [Package Format](#package-format)
- [Examples](#examples)
- [Technical Details](#technical-details)

## Installation

### Dependencies

- CMake >= 3.10.0
- C++17 compatible compiler (GCC, Clang, or MSVC)
- libarchive (for handling `.tar.gz` archives)
- yaml-cpp (automatically fetched via CMake)

#### Installing Dependencies on Linux (Debian/Ubuntu)

```bash
sudo apt-get update
sudo apt-get install cmake g++ libarchive-dev pkg-config
```

#### Installing Dependencies on Linux (Fedora/RHEL)

```bash
sudo dnf install cmake gcc-c++ libarchive-devel pkg-config
```

#### Installing Dependencies on macOS

```bash
brew install cmake libarchive pkg-config
```

## Building from Source

1. Clone the repository:
```bash
git clone <repository-url>
cd openspm-repository
```

2. Create a build directory and configure:
```bash
mkdir -p out/build/gcc
cd out/build/gcc
cmake ../../..
```

3. Build the project:
```bash
cmake --build .
```

The executable `openspm-repo` will be created in the build directory.

## Usage

### General Syntax

```bash
openspm-repo <command> [args...] [flags...]
```

### Commands

#### Initialize a Repository

Creates a new OpenSPM repository with metadata and an empty package index.

**Syntax:**
```bash
openspm-repo init_repo <repo_path> --url <base_url> --name <repo_name> --description <desc> --maintainer <maintainer>
```

**Arguments:**
- `<repo_path>` - Path to the repository directory (must exist)

**Flags:**
- `--url <base_url>` - Base URL where the repository will be hosted
- `--name <repo_name>` - Name of the repository
- `--description <desc>` - Description of the repository
- `--maintainer <maintainer>` - Repository maintainer name/email

**Example:**
```bash
openspm-repo init_repo ./my-repo \
  --url https://my-repo.openspm.org \
  --name "My Package Repository" \
  --description "Custom packages for my organization" \
  --maintainer "admin@example.com"
```

**Output Files:**
- `repository.yaml` - Repository metadata
- `pkg-list.yaml` - Empty package list (initialized)

---

#### Rebuild Package Index

Scans the repository directory for `.tar.gz` package archives, extracts their `pkg.yaml` metadata, and regenerates the package index.

**Syntax:**
```bash
openspm-repo rebuild_index <repo_path>
```

**Arguments:**
- `<repo_path>` - Path to the repository directory

**Example:**
```bash
openspm-repo rebuild_index ./my-repo
```

**What it does:**
1. Reads repository metadata from `repository.yaml`
2. Recursively scans for all `.tar.gz` files in the repository
3. Extracts `pkg.yaml` from each archive
4. Parses package metadata (name, version, description, dependencies, tags)
5. Generates download URLs based on repository base URL and relative paths
6. Writes complete package list to `pkg-list.yaml`

**Output:**
- Updates `pkg-list.yaml` with all discovered packages and their metadata

---

#### Add External Repository URL

Adds an external repository URL to the dependency list in `pkg-list.yaml`.

**Syntax:**
```bash
openspm-repo add_url <repo_path> <packages_url>
```

**Arguments:**
- `<repo_path>` - Path to the repository directory
- `<packages_url>` - URL of the external repository's package list

**Example:**
```bash
openspm-repo add_url ./my-repo https://upstream.openspm.org/pkg-list.yaml
```

**What it does:**
- Adds the external URL to the `depend` array in `pkg-list.yaml`
- Avoids duplicates (checks if URL already exists)

---

## Repository Structure

A typical OpenSPM repository has the following structure:

```
my-repo/
├── repository.yaml          # Repository metadata
├── pkg-list.yaml           # Generated package index
└── packages/               # Package archives (organized as needed)
    ├── acct-6.6.4.tar.gz
    ├── bash-5.1.tar.gz
    └── utils/
        └── curl-7.80.tar.gz
```

### repository.yaml Format

```yaml
url: https://my-repo.openspm.org
name: My Package Repository
description: Custom packages for my organization
maintainer: admin@example.com
```

### pkg-list.yaml Format

```yaml
packages:
  - name: acct
    version: 6.6.4
    description: GNU acct package
    maintainer: ghillie575
    dependencies: []
    tags: bin;linux-x86_64;gnu
    url: https://my-repo.openspm.org/packages/acct-6.6.4.tar.gz
  - name: bash
    version: 5.1
    description: GNU Bourne Again SHell
    maintainer: ghillie575
    dependencies:
      - readline
      - ncurses
    tags: bin;linux-x86_64;gnu;shell
    url: https://my-repo.openspm.org/packages/bash-5.1.tar.gz
depend:
  - https://upstream.openspm.org/pkg-list.yaml
```

## Package Format

Each package archive (`.tar.gz`) must contain a `pkg.yaml` file at the root or in a subdirectory.

### pkg.yaml Format

```yaml
name: acct
version: 6.6.4
description: GNU acct package
maintainer: gnu
dependencies: []
tags: bin;linux-x86_64;
url: https://your-repo.openspm.org/pkg/acct.tar.gz
```

**Fields:**
- `name` - Package name (required)
- `version` - Package version (required)
- `description` - Short description of the package (required)
- `maintainer` - Package maintainer name/email (required)
- `dependencies` - Array of dependency package names (required, can be empty)
- `tags` - Semicolon-separated tags (e.g., architecture, category) (required).
- `url` - Original package URL (optional, overridden by repository URL during index rebuild)

## Examples

### Complete Workflow: Creating and Publishing a Repository

#### 1. Create Repository Directory

```bash
mkdir -p my-repo/packages
cd my-repo
```

#### 2. Initialize Repository

```bash
openspm-repo init_repo . \
  --url https://packages.myorg.com \
  --name "MyOrg Packages" \
  --description "Internal package repository" \
  --maintainer "devops@myorg.com"
```

#### 3. Add Package Archives

Place your `.tar.gz` packages in the repository. Each must contain a `pkg.yaml`:

```bash
# Example: Create a simple package
mkdir -p temp-pkg
cat > temp-pkg/pkg.yaml << EOF
name: hello-world
version: 1.0.0
description: Hello World application
maintainer: dev@myorg.com
dependencies: []
tags: bin;linux-x86_64;example
url: https://packages.myorg.com/packages/hello-world-1.0.0.tar.gz
EOF

# Add your binary/files
cp /path/to/hello-world temp-pkg/

# Create archive
tar czf packages/hello-world-1.0.0.tar.gz -C temp-pkg .
rm -rf temp-pkg
```

#### 4. Rebuild Package Index

```bash
openspm-repo rebuild_index .
```

This will scan all `.tar.gz` files and generate `pkg-list.yaml` with complete metadata.

#### 5. Add Upstream Dependencies (Optional)

```bash
openspm-repo add_url . https://official.openspm.org/pkg-list.yaml
```

#### 6. Publish Repository

Upload the entire directory to your web server:

```bash
rsync -avz --delete . user@server:/var/www/packages.myorg.com/
```

### Updating Repository After Adding Packages

After adding new `.tar.gz` packages to your repository:

```bash
# Rebuild the index
openspm-repo rebuild_index /path/to/repo

# Sync to server
rsync -avz --delete /path/to/repo/ user@server:/var/www/repo/
```

## Technical Details

### Archive Processing

- The tool uses `libarchive` to read `.tar.gz` files
- Searches for `pkg.yaml` at any depth within the archive
- Extracts and parses YAML metadata
- Generates absolute package URLs by combining repository base URL with relative archive paths

### Metadata Generation

- Uses `yaml-cpp` for YAML parsing and generation
- Validates required fields in `pkg.yaml`
- Preserves dependency information
- Handles empty dependency arrays correctly

### URL Construction

Package URLs are constructed as:
```
<repository_base_url> + "/" + <relative_path_to_archive>
```

Example:
- Repository base URL: `https://my-repo.openspm.org`
- Archive location: `./packages/subdir/acct-6.6.4.tar.gz`
- Generated URL: `https://my-repo.openspm.org/packages/subdir/acct-6.6.4.tar.gz`

### Error Handling

- Missing `repository.yaml`: Error, cannot proceed
- Missing `pkg.yaml` in archive: Warning, package skipped
- Invalid YAML format: Error during parsing
- Non-existent repository path: Error

## Development

### Project Structure

```
openspm-repository/
├── CMakeLists.txt              # Build configuration
├── CMakePresets.json           # CMake presets
├── main.cpp                    # Entry point
├── include/
│   ├── openspm.hpp            # Core data structures and functions
│   ├── openspm_repo.hpp       # Repository management functions
│   └── openspm_repo_cli.hpp   # Command-line interface
└── src/
    ├── openspm.cpp            # Metadata parsing/generation
    ├── openspm_repo.cpp       # Repository operations
    └── openspm_repo_cli.cpp   # CLI command processing
```

### Key Components

- **openspm.hpp/cpp**: Core data structures (`PackageInfo`, `RepositoryInfo`) and metadata parsing
- **openspm_repo.hpp/cpp**: Repository operations (init, rebuild index, add URLs, archive reading)
- **openspm_repo_cli.hpp/cpp**: Command-line argument parsing and command dispatch
- **main.cpp**: Entry point and argument preprocessing
