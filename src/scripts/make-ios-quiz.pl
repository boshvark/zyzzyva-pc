#!/usr/bin/perl

use warnings;
use strict;
use Getopt::Long;

my %options;
my $ok = GetOptions(\%options, "zyzdir=s", "lexicon=s", "outdir=s");

# Zyzzyva installation directory
die "Please specify Zyzzyva installation directory with --zyzdir.\n" unless exists $options{'zyzdir'};
my $zyzdir = $options{'zyzdir'};

# Lexicon
die "Please specify lexicon with --lexicon.\n" unless exists $options{'lexicon'};
my $lexicon = $options{'lexicon'};
my $lexicon_location = 'North-American';
if ($lexicon eq 'CSW12') {
    $lexicon_location = 'British';
}

# Output directory
my $outdir = '.';
if (exists $options{'outdir'}) {
    $outdir = $options{'outdir'};
    system 'mkdir', '-p', $outdir;
}

# Create output file name from input list file names
die "Please specify word list files.\n" unless @ARGV;
my $quiz_name = join("_", map { my $f = $_; $f =~ s/^.*\///; $f =~ s/\.txt\z//; $f } @ARGV);

# Read word count for each alphagram in lexicon
my %alphagram_words;
my $lexicon_file = "$zyzdir/data/words/$lexicon_location/$lexicon.txt";
open IN, "<$lexicon_file" or die "Can't open $lexicon_file: $!\n";
while (my $line = <IN>) {
    my ($word) = ($line =~ /(\w+)/);
    next unless length $word;
    $word = uc $word;
    my $alphagram = join('', sort(split '', $word));
    push @{$alphagram_words{$alphagram}}, $word;
}
close IN;

# Create hash of quiz alphagrams
my $total_count = 0;
my %quiz_alphagrams;
while (my $line = <>) {
    my ($word) = ($line =~ /(\w+)/);
    next unless length $word;
    $word = uc $word;
    my $alphagram = join('', sort(split '', $word));
    next if exists $quiz_alphagrams{$alphagram};
    $quiz_alphagrams{$alphagram} = 1;
    $total_count += @{$alphagram_words{$alphagram}};
}


# Write create table SQL
my $sql_file = "$outdir/$quiz_name.sql";
my $zq_file = "$outdir/$quiz_name.zq";
open SQL_OUT, ">$sql_file" or die "Can't write to $sql_file: $!\n";
print SQL_OUT <<EOF;
CREATE TABLE questions (question_index integer, status integer, name text);
CREATE UNIQUE INDEX question_index_index ON questions (question_index);
CREATE INDEX question_status_index ON questions (status);
CREATE TABLE quiz (lexicon text, type integer, current_question integer, num_words integer, method integer, question_order integer);
CREATE TABLE responses (question_index integer, status integer, name text);
CREATE INDEX question_response_index ON responses (question_index);
CREATE UNIQUE INDEX question_response_response_index ON responses (question_index, name);
EOF

# Write question data
my $question_index = 0;
my $first_alphagram;
for my $alphagram (keys %quiz_alphagrams) {
    if ($question_index == 0) {
        $first_alphagram = $alphagram;
    }
    my $question_status = ($question_index == 0 ? 1 : 0);
    print SQL_OUT "INSERT INTO questions (question_index, status, name) VALUES ($question_index, $question_status, '$alphagram');\n";
    ++$question_index;
}

# Write quiz data
print SQL_OUT "INSERT INTO quiz (lexicon, type, current_question, num_words, method, question_order) VALUES ('OWL2', 2, 0, $total_count, 1, 1);\n";

# Write response data
for my $word (@{$alphagram_words{$first_alphagram}}) {
    print SQL_OUT "INSERT INTO responses (question_index, status, name) VALUES (0, 0, '$word');\n";
}

close SQL_OUT;

# Create Zyzzyva quiz file and remove temp SQL file
system "rm -f $zq_file";
system "cat $sql_file | sqlite3 $zq_file";
system "rm $sql_file";
