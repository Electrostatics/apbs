#!/bin/csh -xv

# get the release ids and names
curl -i https://api.github.com/repos/Electrostatics/apbs-pdb2pqr/releases -H "Accept:application/vnd.github.manifold-preview+json"

# "id": 356197,
# "name": "pdb2pqr-linux-bin64-2.0.0.tar.gz",
curl -i https://api.github.com/repos/Electrostatics/apbs-pdb2pqr/releases/assets/356197 -H "Accept: application/vnd.github.manifold-preview+json" | grep download_count

# "id": 356198,
# "name": "pdb2pqr-osx-bin64-2.0.0.tar.gz",
curl -i https://api.github.com/repos/Electrostatics/apbs-pdb2pqr/releases/assets/356198 -H "Accept: application/vnd.github.manifold-preview+json" | grep download_count

# "id": 356199,
# "name": "pdb2pqr-windows-bin64-2.0.0.zip",
curl -i https://api.github.com/repos/Electrostatics/apbs-pdb2pqr/releases/assets/356199 -H "Accept: application/vnd.github.manifold-preview+json" | grep download_count


