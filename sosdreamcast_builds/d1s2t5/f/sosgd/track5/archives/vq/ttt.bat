tarcman montana.tc -x SkyLeft:3df-565
texus -rn -mn -dn -o .tga SkyLeft.3df
pvrconv SkyLeft.tga -5 -nvm -v3 -f
tarcman montana.tc -a SkyLeft:3df-vq SkyLeft.pvr
tarcman montana.tc -x SkyRight:3df-565
texus -rn -mn -dn -o .tga SkyRight.3df
pvrconv SkyRight.tga -5 -nvm -v3 -f
tarcman montana.tc -a SkyRight:3df-vq SkyRight.pvr
