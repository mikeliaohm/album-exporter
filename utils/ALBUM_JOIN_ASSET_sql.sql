select Z_27ASSETS.Z_3ASSETS, ZASSET.Z_PK
from Z_27ASSETS
INNER JOIN ZASSET
ON Z_27ASSETS.Z_3ASSETS = ZASSET.Z_PK and Z_27ASSETS.Z_27ALBUMS = 103