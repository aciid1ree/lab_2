set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'speedup_comparison.png'
set title 'Ускорение метода Ньютона с OpenMP\nУравнение: 1x^2 - 0x = 0'
set xlabel 'Количество потоков'
set ylabel 'Ускорение'
set grid
set key top left
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5
set style line 2 lc rgb '#dd181f' lt 1 lw 2 dt 2
plot 'speedup_data.txt' using 1:3 title 'Реальное ускорение' with linespoints ls 1, \
     'speedup_data.txt' using 1:4 title 'Линейное ускорение' with lines ls 2
