


/***********************************/		
         BC_Chip8Test
/***********************************/	


DESCRIPTION: Permet de tester les sauts conditionnels, les op�rations math�matiques et logiques de la Chip 8

AUTEUR: BestCoder

CONTACT:  mail: bestcoder@ymail.com    

COPYRIGHT: Vous pouvez utiliser et redistribuer cette rom � volont� tant que les cr�dits me sont attribu�s. Aucun usage commercial n'est autoris� sans mon accord!

VERSION: FR   01/07/2011



/***********************************/	
	COMMENT L'UTILISER:
/***********************************/	

Chaque erreur est accompagn�e d'un nombre qui d�signe l'opcode concern�. Si tous les tests sont positifs, vous verrez afficher � l'�cran "BON".


/***********************************/	
	CORRESPONDANCES
/***********************************/	


E 01 :  3XNN  v�rifier que la condition de saut est juste

E 02 :  5XY0 v�rifier que la condition de saut est juste

E 03 :  4XNN v�rifier que la condition de saut est juste

E 04 :  7XNN v�rifier le r�sultat de l'addition

E 05 :  8XY5 v�rifier que VF est mis � 0 quand il y a un emprunt

E 06 :  8XY5 v�rifier que VF est mis � 1 quand il n'y a pas d'emprunt

E 07 :  8XY7 v�rifier que VF est mis � 0 quand il y a un emprunt

E 08 :  8XY7 v�rifier que VF est mis � 1 quand il n'y a pas d'emprunt

E 09 :  8XY1 v�rifier le r�sultat du OR

E 10 :  8XY2 v�rifier le r�sultat du AND

E 11 :  8XY3 v�rifier le r�sultat du XOR

E 12 :  8XYE v�rifier que VF prend le MSB (bit de poids fort ou le plus � gauche) avant le d�calage et que VF ne prend pas 0 � tous les coups

E 13 :  8XYE v�rifier que VF prend le MSB (bit de poids fort ou le plus � gauche) avant le d�calage et que VF ne prend pas 1 � tous les coups

E 14 :  8XY6 v�rifier que VF prend le LSB (bit de poids faible ou le plus � droite) avant le d�calage et que VF ne prend pas 0 � tous les coups

E 15 :  8XY6 v�rifier que VF prend le LSB (bit de poids faible ou le plus � droite) avant le d�calage et que VF ne prend pas 1 � tous les coups

E 16 :  FX55 et FX65  v�rifier que ces deux opcodes sont bien impl�ment�s. L'erreur peut venir de l'une comme de l'autre ou les deux � la fois sont d�ffectueux.

E 17 :  FX33   le calcul de la repr�sentation binaire est mal effectu� ou le r�sultat est mal stock�. ou encore l'opcode FX65 d�conne ou le FX1E.


/***************************/
	Bon d�bogage
/***************************/
