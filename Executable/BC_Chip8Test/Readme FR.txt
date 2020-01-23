


/***********************************/		
         BC_Chip8Test
/***********************************/	


DESCRIPTION: Permet de tester les sauts conditionnels, les opérations mathématiques et logiques de la Chip 8

AUTEUR: BestCoder

CONTACT:  mail: bestcoder@ymail.com    

COPYRIGHT: Vous pouvez utiliser et redistribuer cette rom à volonté tant que les crédits me sont attribués. Aucun usage commercial n'est autorisé sans mon accord!

VERSION: FR   01/07/2011



/***********************************/	
	COMMENT L'UTILISER:
/***********************************/	

Chaque erreur est accompagnée d'un nombre qui désigne l'opcode concerné. Si tous les tests sont positifs, vous verrez afficher à l'écran "BON".


/***********************************/	
	CORRESPONDANCES
/***********************************/	


E 01 :  3XNN  vérifier que la condition de saut est juste

E 02 :  5XY0 vérifier que la condition de saut est juste

E 03 :  4XNN vérifier que la condition de saut est juste

E 04 :  7XNN vérifier le résultat de l'addition

E 05 :  8XY5 vérifier que VF est mis à 0 quand il y a un emprunt

E 06 :  8XY5 vérifier que VF est mis à 1 quand il n'y a pas d'emprunt

E 07 :  8XY7 vérifier que VF est mis à 0 quand il y a un emprunt

E 08 :  8XY7 vérifier que VF est mis à 1 quand il n'y a pas d'emprunt

E 09 :  8XY1 vérifier le résultat du OR

E 10 :  8XY2 vérifier le résultat du AND

E 11 :  8XY3 vérifier le résultat du XOR

E 12 :  8XYE vérifier que VF prend le MSB (bit de poids fort ou le plus à gauche) avant le décalage et que VF ne prend pas 0 à tous les coups

E 13 :  8XYE vérifier que VF prend le MSB (bit de poids fort ou le plus à gauche) avant le décalage et que VF ne prend pas 1 à tous les coups

E 14 :  8XY6 vérifier que VF prend le LSB (bit de poids faible ou le plus à droite) avant le décalage et que VF ne prend pas 0 à tous les coups

E 15 :  8XY6 vérifier que VF prend le LSB (bit de poids faible ou le plus à droite) avant le décalage et que VF ne prend pas 1 à tous les coups

E 16 :  FX55 et FX65  vérifier que ces deux opcodes sont bien implémentés. L'erreur peut venir de l'une comme de l'autre ou les deux à la fois sont déffectueux.

E 17 :  FX33   le calcul de la représentation binaire est mal effectué ou le résultat est mal stocké. ou encore l'opcode FX65 déconne ou le FX1E.


/***************************/
	Bon débogage
/***************************/
