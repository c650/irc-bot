#!/usr/bin/env ruby

require 'pry'
require 'json'
require 'mechanize'
require 'rubygems'
require 'open-uri'

puts "Looking up #{ARGV[0]}"

if ARGV[0].length < 3
	puts "invalid."
	exit(1)
end

agent = Mechanize.new
agent.user_agent = "curl/7.43.0"

begin
	url = "http://freegeoip.net/json/#{ARGV[0].gsub(" ","")}"
	uri = URI(url)

	data = `curl #{uri}`

	json = JSON.parse(data)
	weather = agent.get("http://wttr.in/#{json["city"] ? json["city"] : json["country_name"]}").body.split("\n")

	puts "Location: #{json["city"]}, #{json["region_code"]}, #{json["country_name"]}"

	if (json["city"] != "")
		puts "Weather right now: "
		for i in 1..6
			weather[i].gsub!(/\e\[(\d+);*(\d*);*(\d*)m/, '')
		end
		puts weather[1..6]
	else 
		puts "Weather unavailable."
	end
rescue
	puts "Failed to look up #{ARGV[0]}."
end
