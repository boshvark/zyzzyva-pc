#!/usr/bin/env ruby

require 'scrabble_word'

print "Word file: "
fname = STDIN.gets.chomp

#fname = '/home/mthelen/scrabble/data/ospd3/3-words.txt'
puts "File is: #{fname}"

words = ScrabbleWord.read_words(fname)

keep_going = true
while keep_going
    word = words[rand(words.length)]
    correct = nil

    until correct
        puts "Alphagram: #{word.alphagram}"
        guess = gets.chomp.upcase
        if guess.eql?(word.name) or guess.empty?
            puts (guess.empty? ? "Giving up?" : "Correct!") +
                 " Word: #{word.name}: #{word.definition}"
            correct = true
        elsif guess == 'C'
            puts "Definition: #{word.definition}"
        elsif guess == 'Q'
            correct = true
            keep_going = nil
        else
            puts "Nope, try again..."
        end
    end
end

puts "Bye..."
