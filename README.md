# PeacockPS5

[![Build](https://github.com/Askabis/PeacockPS5/actions/workflows/tooling.yml/badge.svg)](https://github.com/Askabis/PeacockPS5/actions/workflows/tooling.yml)
[![License: GPL-3.0-or-later](https://img.shields.io/badge/license-GPL--3.0--or--later-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/Askabis/PeacockPS5?display_name=tag)](https://github.com/Askabis/PeacockPS5/releases/latest)

**[Français](README.fr.md) | [English](README.en.md)**

PeacockPS5 est une application homebrew PS5 native qui vérifie la connexion entre une console et une instance locale de [Peacock](https://github.com/thepeacockproject/Peacock) sur le même réseau.

PeacockPS5 is a native PS5 homebrew application that checks connectivity between a console and a local [Peacock](https://github.com/thepeacockproject/Peacock) instance on the same network.

> Prototype actuel : test TCP/HTTP, affichage du résultat et journal local. Il ne redirige pas encore HITMAN vers Peacock.
>
> Current prototype: TCP/HTTP probe, on-screen result, and local log. It does not yet redirect HITMAN to Peacock.

La version `01.001.000` ajoute un daemon réseau indépendant destiné à rester actif pendant le lancement d'un jeu.

Version `01.001.000` adds an independent network daemon designed to remain active while a game is running.

## Installation

- **Français : [installer PeacockPS5 sur PS5](docs/INSTALLATION.fr.md)**
- **English: [install PeacockPS5 on PS5](docs/INSTALLATION.en.md)**
- **Français : [daemon en arrière-plan](docs/BACKGROUND_DAEMON.fr.md)**
- **English: [background daemon](docs/BACKGROUND_DAEMON.en.md)**

## Cadre / Scope

Projet destiné à l'interopérabilité légale sur du matériel et des jeux contrôlés par l'utilisateur. Aucun exploit PS5, contournement de protection, patch mémoire de jeu, outil de piratage ou package retail signé n'est fourni.

For lawful interoperability research on hardware and games controlled by the user. No PS5 exploit, protection bypass, game-memory patcher, piracy tooling, or signed retail package is provided.

GPL-3.0-or-later. Voir / See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
