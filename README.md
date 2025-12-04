# ChondritesViewer
Allows to visualise and segment chondrites scanned through a SEM with BSE and EDS maps


Color palette:
/*
#577277 
#468232
#3c5e8b 
#73bed3
#a8ca58
#e7d5b3 
#7a367b
#a53030
#da863e
#c65197 
#884b2b
*/


Vérité terrain existante:
- masque chondre vs reste impréçis, pas propre (on peut superposer avec bse et montrer celle pas prise en compte)
- masque par objet mais que sur une zone
- masque pixel wise que sur une zone mais avec bcp de bruit (histogramme nombre de pixels par classe + moyenne/variance spectre pour voir si cohérent + dire que classe pas toujours liés à celle qu'il nous demande d'extraire).

Approche avec ML classique, bon res mais random forest assez lent. Image très grande donc peut être CNN/ViT. Avantage CNN = transfert learning. Vue qu'on est que sur une météorite et pas sur les autres, si on est bon sur une peut espérer avec peu de données e^tre bon sur les autres.

Pistes:
- voir pour ajouter plus de données
-  