

def drawcirclesA(expr na, nb, j) =
    numeric ra, dd; ra := 7mm; dd := 2cm;
    path cca;
    pair dx, dy;
    dx := dir( 0) scaled dd;
    dy := dir(60) scaled dd;
    for i = na upto nb:
        cca := fullcircle scaled ra;
        draw cca
            shifted (dx scaled i) shifted (dy scaled j);
    endfor;
enddef;
def drawcirclesC(expr na, nb, j) =
    numeric ra, rb, rc, dd; ra := 7mm; rb := 5mm; rc:=9.4mm; dd := 2cm;
    path cca, ccb;
    pair dx, dy;
    dx := dir( 0) scaled dd;
    dy := dir(60) scaled dd;
    for i = na upto nb:
        cca := fullcircle scaled rc rotated -135;
        ccb := fullcircle rotated 90 scaled ra shifted (dir(-135) scaled .5*(ra+rb));
        ccb := ccb cutafter cca;
        ccb := ccb cutbefore cca;
        cca := cca cutafter ccb;
        cca := cca cutbefore ccb;
        draw cca
            shifted (dx scaled i) shifted (dy scaled j);
        draw ccb
            shifted (dx scaled i) shifted (dy scaled j);
    endfor;
enddef;
def drawcirclesB(expr na, nb, j) =
    numeric r, dd; r := 15mm; dd := 2cm;
    path cca, ccb, ccd;
    pair dx, dy;
    dx := dir( 0) scaled dd;
    dy := dir(60) scaled dd;
    for i = na upto nb:
        ccb := fullcircle rotated 45 scaled r;
        cca := (fullcircle rotated 150 scaled 0.3 shifted (dir(45) scaled 0.5)) scaled r;
        cca := cca cutbefore ccb;
        ccb := ccb cutafter cca;
        cca := cca cutafter ccb;
        draw cca
            shifted (dx scaled i) shifted (dy scaled j);
        draw (ccb cutbefore cca)
            shifted (i*2cm,0pt) shifted (dir(60) scaled j*2cm);
    endfor;
enddef;

beginfig(1);

drawcirclesA(1, 12, -1);
drawcirclesA(0, 12,  0);
drawcirclesA(0, 11,  1);
drawcirclesA(1,  7,  2);
draw unitsquare xscaled 30cm yscaled 81mm shifted (-2.7cm, -3.1cm);

endfig;

beginfig(2);

drawcirclesA(1, 12, -1);
drawcirclesA(0, 12,  0);
drawcirclesA(0, 11,  1);
drawcirclesA(1,  7,  2);

drawcirclesB(1, 12, -1);
drawcirclesB(0, 12,  0);
drawcirclesB(0, 11,  1);
drawcirclesB(1,  7,  2);
draw unitsquare xscaled 30cm yscaled 81mm shifted (-2.7cm, -3.1cm);

endfig;

beginfig(3);

drawcirclesC(1, 12, -1);
drawcirclesC(0, 12,  0);
drawcirclesC(0, 11,  1);
drawcirclesC(1,  7,  2);
draw unitsquare xscaled 30cm yscaled 81mm shifted (-2.7cm, -3.1cm);

endfig;

end
