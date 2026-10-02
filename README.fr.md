# PeacockPS5

[Accueil](README.md) | **Français** | [English](README.en.md)

PeacockPS5 est un prototype homebrew PS5 natif. Il vérifie qu'une console appartenant à l'utilisateur peut joindre une instance locale de [Peacock](https://github.com/thepeacockproject/Peacock) sur le LAN.

L'application charge une configuration, ouvre une connexion TCP avec un délai limité, envoie une requête HTTP, affiche la première ligne de réponse et écrit un journal dans `/download0/peacock_probe.log`.

## État actuel

- Sonde IPv4 TCP/HTTP configurable.
- Configuration Peacock modifiable sans recompilation.
- Résultat visible à l'écran et journal local.
- Aucune modification de HITMAN et aucune redirection du jeu à ce stade.

## Installer

Suivez le guide court : **[Installation sur PS5](docs/INSTALLATION.fr.md)**.

La version précompilée est disponible dans [GitHub Releases](https://github.com/Askabis/PeacockPS5/releases/latest). Le projet produit une application homebrew de développement, pas un fichier PKG retail.

## Configuration Peacock

Configuration par défaut dans `assets/peacock.conf` :

```ini
host=192.168.1.10
port=80
path=/authentication/api/configuration/Init
timeout_ms=3000
```

Sur la console, `/download0/peacock.conf` remplace ces valeurs sans reconstruire l'application. `host` doit actuellement être une adresse IPv4 du LAN.

## Compiler

Sous Linux ou WSL :

```bash
make doctor
make
```

Le résultat complet se trouve dans `dist/PPSA99999/`. Consultez [GETTING_STARTED.md](docs/GETTING_STARTED.md) pour l'environnement de compilation et [DEPLOYMENT.md](docs/DEPLOYMENT.md) pour le flux de développement.

## Périmètre

Ce dépôt concerne l'interopérabilité légale avec du matériel et des jeux contrôlés par l'utilisateur. Il ne contient ni chaîne d'exploitation PS5, ni élévation de privilèges, ni signature de package retail, ni outil de piratage, ni patch mémoire de HITMAN.

Les [notes techniques Peacock](docs/PEACOCK_PROTOCOL_NOTES.md) séparent le protocole HTTP de Peacock des composants Windows de PeacockPatcher.

Base inspirée de [ps5-native-app-boilerplate](https://github.com/blackbearreloaded/ps5-native-app-boilerplate). Licence GPL-3.0-or-later ; voir [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
