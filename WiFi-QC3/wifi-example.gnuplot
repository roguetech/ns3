set terminal postscript portrait enhanced lw 2 "Helvetica" 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.eps"
#set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Throughput [Mbps]"
set yrange [0:5]

plot "throughput.data" with lines title "WiFi Throughput vs. Distance"

#-------------------------------------------------------
set out "wifi-throughput-5.eps"
#set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Throughput [Mbps]"
set yrange [0:5]

plot "throughput-5.data" with lines title "WiFi Throughput vs. Distance"

#-------------------------------------------------------
set out "wifi-throughput-10.eps"
#set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Throughput [Mbps]"
set yrange [0:5]

plot "throughput-10.data" with lines title "WiFi Throughput vs. Distance"

#-------------------------------------------------------
set out "wifi-throughput-15.eps"
#set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Throughput [Mbps]"
set yrange [0:5]

plot "throughput-15.data" with lines title "WiFi Throughput vs. Distance"

#-------------------------------------------------------
set out "wifi-throughput-20.eps"
#set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Throughput [Mbps]"
set yrange [0:5]

plot "throughput-20.data" with lines title "WiFi Throughput vs. Distance"


#-------------------------------------------------------
set out "wifi-delay.eps"
#set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Delay [ms]"
set yrange [0:5]

plot "delay.data" with lines title "WiFi Delay vs. Distance"

#-------------------------------------------------------
set out "wifi-delay-5.eps"
#set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Delay [ms]"
set yrange [0:1000]

plot "delay-5.data" with lines title "WiFi Delay vs. Distance"

#-------------------------------------------------------
set out "wifi-delay-10.eps"
#set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Delay [ms]"
set yrange [0:1000]

plot "delay-10.data" with lines title "WiFi Delay vs. Distance"

#-------------------------------------------------------
set out "wifi-delay-15.eps"
#set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Delay [ms]"
set yrange [0:1000]

plot "delay-15.data" with lines title "WiFi Delay vs. Distance"

#-------------------------------------------------------
set out "wifi-delay-20.eps"
#set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Delay [ms]"
set yrange [0:1000]

plot "delay-20.data" with lines title "WiFi Delay vs. Distance"

#-------------------------------------------------------
set out "wifi-loss.eps"
#set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:100]

plot "loss.data" with lines title "WiFi PLR vs. Distance"

#-------------------------------------------------------
set out "wifi-loss-5.eps"
#set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:100]

plot "loss-5.data" with lines title "WiFi PLR vs. Distance"

#-------------------------------------------------------
set out "wifi-loss-10.eps"
#set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:100]

plot "loss-10.data" with lines title "WiFi PLR vs. Distance"

#-------------------------------------------------------
set out "wifi-loss-15.eps"
#set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:100]

plot "loss-15.data" with lines title "WiFi PLR vs. Distance"

#-------------------------------------------------------
set out "wifi-loss-20.eps"
#set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:150]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:100]

plot "loss-20.data" with lines title "WiFi PLR vs. Distance"

