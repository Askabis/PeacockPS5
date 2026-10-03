# HITMAN Memory Diagnostic / Diagnostic memoire HITMAN

## Francais

Payload experimental, **pas un patch PSN**. Cherche uniquement `PPSA01769`,
lit trois marqueurs reseau et cinq zones de code fixes, puis quitte. Ne modifie ni la memoire du jeu,
ni ses fichiers, ni le reseau. Aucun appel reseau n'est effectue.
Le SDK utilise les privileges deja disponibles dans l'environnement homebrew ;
son lecteur mdbg ajuste temporairement les privileges du processus de diagnostic.
Ce n'est donc pas un outil sans risque ni une demonstration de compatibilite firmware.

1. Laisser HITMAN hors ligne au menu, sans choisir Se connecter.
2. Envoyer `peacockps5-inspect.elf` au chargeur de payload deja operationnel.
3. Lire `/data/peacockps5/inspect.log` via le serveur FTP existant.
4. Si `CODE_SNAPSHOT_V1_OK` apparait, recuperer
   `/data/peacockps5/auth-code-v1.bin` hors du depot et comparer avec
   `python tools/verify-hitman-snapshot.py CHEMIN/auth-code-v1.bin`.

Le fichier contient 3 635 octets de code executable, pas les donnees de session.
Ne jamais le publier. Les cinq empreintes SHA-256 attendues ont ete calculees
sur la copie locale analysee. Toute difference ou taille incorrecte est un echec.
La lecture ne suspend pas le jeu et ne constitue pas un instantane atomique.

`READ_PROBE_OK` signifie uniquement que les trois marqueurs ont ete lus.
Un message `STOP` interdit de poursuivre avec des hypotheses sur les adresses.
Le journal est remplace a chaque execution. Aucun identifiant PSN, jeton ou
contenu de memoire arbitraire n'est journalise. Le daemon habituel est independant.

Les adresses proviennent d'une copie locale dont le SHA-256 est
`570294cc0bf9578a2ca2667dc74bcbe6e8178d58fd9b277de6465f704c213078`.
Trois marqueurs ne verifient pas ce hash et n'autorisent aucune modification.
Ne pas essayer une connexion du jeu sur un reseau ouvert en supposant que
ce payload bloque le PSN : il ne le fait pas.

Compilation : workflow manuel **Read-only diagnostic**, SDK public epingle
par `tools/setup-native-dependencies.sh`. Aucun binaire du jeu n'est inclus.

## English

Experimental payload, **not a PSN patch**. Finds only `PPSA01769`, reads three
network markers and five fixed code regions, then exits. No game-memory writes, game-file edits or network
calls. The SDK's mdbg reader temporarily adjusts this diagnostic process's
credentials using the existing homebrew environment. Firmware compatibility
and risk-free execution are not assumed.

Keep HITMAN at its offline menu, send `peacockps5-inspect.elf` through your
existing payload loader, then retrieve `/data/peacockps5/inspect.log` via FTP.
`READ_PROBE_OK` verifies only those reads, not authentication or the full build.
Stop on any `STOP` result. Logs are overwritten on each run; no PSN identifiers,
tokens or arbitrary memory contents are logged. This payload does not block PSN.

On `CODE_SNAPSHOT_V1_OK`, retrieve `/data/peacockps5/auth-code-v1.bin` outside
the repository and run `python tools/verify-hitman-snapshot.py PATH/auth-code-v1.bin`.
This 3,635-byte executable-code snapshot must never be published. The verifier
rejects incorrect length or SHA-256 hashes. Reads are not atomic and do not
prove execution, full-build identity, or successful authentication.

Build with the manual **Read-only diagnostic** workflow. No game binary is
included. The regular connectivity daemon is unchanged.
