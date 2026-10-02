# Installer PeacockPS5

[Accueil](../README.md) | **Français** | [English](INSTALLATION.en.md)

PeacockPS5 est une application homebrew de développement, pas un payload d'exploitation ni un PKG retail. Ce guide suppose que votre propre PS5 dispose déjà d'un environnement homebrew compatible, de ShadowMountPlus (ou d'un chargeur équivalent acceptant les applications en dossier) et d'un serveur FTP actif.

Compatibilité actuellement validée par la base native : firmwares 6.02 et 12.70 avec ShadowMountPlus. Les autres combinaisons ne sont pas encore vérifiées.

## 1. Préparer Peacock sur le PC

1. Démarrez Peacock sur un PC du même réseau local que la PS5.
2. Faites-le écouter sur l'adresse LAN du PC, pas uniquement sur `127.0.0.1`. Peacock utilise par défaut le port `80` et accepte `HOST=0.0.0.0`.
3. Autorisez ce port dans le pare-feu privé du PC.
4. Notez l'adresse IPv4 du PC, par exemple `192.168.1.10`.

Depuis un autre appareil du LAN, ouvrez `http://ADRESSE_DU_PC/authentication/api/configuration/Init`. Une réponse HTTP, même une erreur applicative, confirme que le service est joignable.

## 2. Télécharger et configurer l'application

1. Ouvrez la [dernière version](https://github.com/Askabis/PeacockPS5/releases/latest).
2. Téléchargez `PPSA99999.zip`, puis extrayez-le. Vous devez obtenir un dossier `PPSA99999` contenant notamment `eboot.bin`, `sce_sys/` et `assets/`.
3. Ouvrez `PPSA99999/assets/peacock.conf` et remplacez `host` par l'adresse IPv4 du PC :

```ini
host=192.168.1.10
port=80
path=/authentication/api/configuration/Init
timeout_ms=3000
```

## 3. Copier sur la PS5

1. Sur votre environnement homebrew existant, lancez le serveur FTP et relevez l'adresse IP de la PS5.
2. Avec un client FTP, connectez-vous à cette adresse sur le port `2121` (valeur courante), avec l'identifiant `anonymous` si votre serveur n'en impose pas d'autre.
3. Envoyez le dossier `PPSA99999` entier vers `/data/homebrew/`.
4. Vérifiez que le chemin final est exactement `/data/homebrew/PPSA99999/eboot.bin`. Ne copiez pas seulement `eboot.bin` et n'envoyez pas le ZIP lui-même.
5. Demandez à ShadowMountPlus ou à votre chargeur de rescanner les applications, puis attendez son message de confirmation.

## 4. Lancer et vérifier

Lancez **Peacock PS5 Probe** depuis la section Jeux. L'écran doit afficher la cible, le résultat du probe et la première ligne HTTP. `HTTP/1.1 200`, `3xx` ou `4xx` confirme que Peacock est joignable ; un timeout indique généralement une mauvaise IP, un port bloqué ou Peacock limité à localhost.

Le journal est écrit dans `/download0/peacock_probe.log`. Pour changer la cible plus tard sans renvoyer l'application, placez un fichier `peacock.conf` dans `/download0/` avec les mêmes quatre clés, puis relancez PeacockPS5.

Ce test confirme uniquement la connectivité PS5 vers Peacock. Il ne configure pas encore HITMAN pour utiliser Peacock.
