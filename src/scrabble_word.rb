class ScrabbleWord
    attr_reader :name, :definition

    def initialize(name, definition)
        @name = name
        @definition = definition
    end

    def alphagram()
        name.split(//).sort.join
    end

    def ScrabbleWord.read_words(fname)
        words = []
        File.open(fname) do |file|
            file.each_line do |line|
                word, definition = line.chomp.split(%r{\s+}, 2)
                words << ScrabbleWord.new(word, definition)
            end
        end
        words
    end
end
