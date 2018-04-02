printArray(): non parallélisée, affichage final de la sous-séquence maximale.
allocateTablo(): non parallélisée, allocation d'une zone mémoire pour un tableau.
freeTablo(): non parallélisée, libération d'une zone mémoire allouée pour un tableau.
get_height(): non parallélisée, simple calcul de la hauteur d'un arbre à partir de la taille du tableau (un log2).
fill_dest(): parallélisée, remplissage de la seconde moitié du tableau de destination par le tableau source.
montee(): parallélisée, phase de montée pour sum-prefix et sum-suffix.
monteeMax(): parallélisée, phase de montée pour max-prefix et max-suffix.
descente(): parallélisée, phase de descente pour sum-prefix.
descenteSuff(): parallélisée, phase de descente pour sum-suffix.
descentePreMax(): parallélisée, phase de descente pour max-prefix.
descenteSuffMax(): parallélisée, phase de descente pour max-suffix.
final(): parallélisée, phase finale pour sum-prefix et sum-suffix.
finalMax(): parallélisée, phase finale pour max-prefix et max-suffix.
buildOut(): parallélisée, construction du tableau M.
findMax(): parallélisée, recherche du max dans le tableau M et des bornes de la séquence correspondante.
readArray(): non parallélisée, lecture du fichier et décodage des entiers. Le décodage des entiers pourrait être parallélisé après avoir déterminé l'emplacement de chacun d'eux dans le fichier (on pourrait procéder par bloc pour réduire l'empreinte mémoire).
main(): non parallélisée, exécution séquentielle des étapes. Les étapes 1 et 2 d'une part et 3 et 4 d'autre part pourraient être exécutées en parallèle puisqu'elles sont indépendantes.

Le code des différentes variantes de montee, descente et final est très similaire mais est dupliqué pour des raisons de performance.
