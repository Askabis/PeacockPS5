# Daemon PeacockPS5 en arrière-plan

[Accueil](../README.md) | **Français** | [English](BACKGROUND_DAEMON.en.md)

`peacockps5-daemon.elf` est un payload réseau indépendant de l'application graphique. Lorsqu'il est lancé par un `ps5-payload-elfldr` déjà actif, il reste dans son propre processus pendant le lancement d'un jeu.

Cette première version ne modifie pas HITMAN et ne lit pas sa mémoire. Elle vérifie uniquement que Peacock reste joignable pendant que le jeu fonctionne.

## Préparer la configuration

Créez `/data/peacockps5/peacock.conf` sur la PS5 :

```ini
host=192.168.1.102
port=80
path=/authentication/api/configuration/Init
interval_seconds=15
```

Le daemon relit ce fichier avant chaque test. La valeur minimale de `interval_seconds` est de 5 secondes.

## Lancer avec PS5Upload

1. Vérifiez que l'elfldr persistant écoute sur le port `9021`.
2. Dans PS5Upload, ouvrez **Send payload**.
3. Sélectionnez `peacockps5-daemon.elf` et utilisez le port `9021`.
4. Attendez les notifications **PeacockPS5 daemon started**, puis **Peacock reachable**.
5. Ne renvoyez pas le daemon une seconde fois pendant la même session.

Le dernier état se trouve dans `/data/peacockps5/status.txt`. L'historique se trouve dans `/data/peacockps5/daemon.log`.

## Test avec HITMAN

1. Laissez Peacock actif sur le PC.
2. Lancez le daemon et attendez **Peacock reachable**.
3. Lancez HITMAN World of Assassination normalement.
4. Jouez pendant au moins une minute.
5. Revenez au menu, puis vérifiez que `daemon.log` contient encore des lignes ajoutées pendant l'exécution du jeu.

Ce test valide uniquement la survie du processus et la connectivité réseau en arrière-plan. Il ne redirige pas encore HITMAN vers Peacock.
