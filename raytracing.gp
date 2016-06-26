reset
set ylabel 'time(sec)'
set style fill  solid 
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'


plot [:][:3]'time.txt' using 2:xtic(1) with histogram title 'time ',\
'' using ($0+0.2):($2+0.05):2 with labels title ' '
