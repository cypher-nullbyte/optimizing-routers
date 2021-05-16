
require 'socket'
starttime=Process.clock_gettime(Process::CLOCK_MONOTONIC)

s = TCPSocket.new 'localhost', 9999

s.write("/tmp/testfiles/#{ARGV[0]}.c\n")

# s.each_line do |line|
#     puts line
# end

s.close

endtime = Process.clock_gettime(Process::CLOCK_MONOTONIC)
endtime=Process.clock_gettime(Process::CLOCK_MONOTONIC)
elapsed =endtime-starttime  
puts "Elapsed: #{elapsed} (#{ARGV[0]})\n"