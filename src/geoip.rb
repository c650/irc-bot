#!/usr/bin/env ruby

require 'pry'
require 'json'
require 'mechanize'
require 'rubygems'
require 'open-uri'

puts "Looking up #{ARGV[0]}"

agent = Mechanize.new
agent.user_agent = "curl/7.43.0"

url = "http://freegeoip.net/json/#{ARGV[0].gsub(" ","")}"
uri = URI(url)

data = `curl #{uri}`

json = JSON.parse(data)
weather = agent.get("http://wttr.in/#{json["city"]}").body.split("\n")

puts "Location: #{json["city"]}, #{json["region_code"]}, #{json["country_name"]}"

puts "Weather right now: "
puts weather[1..6]
puts ""
