#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long; Getopt::Long::Configure('bundling');


my @COPY_ARGV      =  @ARGV;
my $HELP           =  0;
my $INPUT_FILE     =  '';
my $PROG           =  $0;

$PROG              =~ s#.*/##;

GetOptions(
            'h+'   => \$HELP,
            'i=s'  => \$INPUT_FILE,
          );

          
sub usage($)
{
  my($help)        =  @_;

  print("\n$PROG " . join(' ', @COPY_ARGV) . "\n") if (! $help);

  print("\n");
  print("  Usage: $0 [-h] | [-i input_file]\n\n");
  print("    -h = Help\n");
  print("    -i = Input file to process ($INPUT_FILE)\n");
  print("\n");

  exit(0) if ($help);

  return;
}


sub get_title() {
    my $title = '';
    open(FD, $INPUT_FILE) || die("Can't open $INPUT_FILE\n");
    while(<FD>) {
      chomp();
      last if m/^={20,}/;
      $title .= $_;
    }
    close(FD);
    return $title;
}


sub get_description() {
    my @descriptions;
    my $description;
    my $skip;
    open(FD, $INPUT_FILE) || die("Can't open $INPUT_FILE\n");
    while(<FD>) {
      last until m/^={20,}/;
    }
    $skip = <FD>;
    $skip = <FD>;
    while(<FD>) {
      last if m/^Input File/;
      chomp();
      push(@descriptions, '        "' . $_ . '"');
    }
    close(FD);
    $description = '[ ' . "\n" . join(",\n", @descriptions) . "\n" . '    ]';
    return $description;
}


sub get_table_header() {
    my $header;
    my @fields;
    my @header;
    open(FD, $INPUT_FILE) || die("Can't open $INPUT_FILE\n");
    while(<FD>) {
      chomp();
      if (m/^Input File\|/) {
        @fields = split(/\s*\|\s*/, $_);
        last;
      }
    }
    foreach (@fields) {
      push(@header, '        "' . $_ . '"');
    }
    close(FD);
    $header = '[ ' . "\n" . join(",\n", @header) . "\n" . '    ]';
    return $header;
}


sub get_table_rows() {
    my @rows;
    open(FD, $INPUT_FILE) || die("Can't open $INPUT_FILE\n");
    while(<FD>) {
      last if m/^Input File\|/;
    }
    my $skip = <FD>;
    while(<FD>) {
      last if m/^\s*$/;
      if (m/.*?\|/) {
          chomp();
          push @rows, $_;
      }
    }
    close(FD);
    return @rows;
}


sub get_notes() {
    my $note;
    my @notes;
    my $skip;
    open(FD, $INPUT_FILE) || die("Can't open $INPUT_FILE\n");
    while(<FD>) {
      last if m/^Input File\|/;
    }
    $skip = <FD>;
    while(<FD>) {
      last if m/^\s*$/;
    }
    $skip = <FD>;
    $skip = <FD>;
    while(<FD>) {
      chomp();
      push(@notes, '        "' . $_ . '"');
    }
    close(FD);
    $note = '[ ' . "\n" . join(",\n", @notes) . "\n" . '    ]';
    return $note;
}


sub get_output() {
  my $output;
  my $title        = get_title();
  my $description  = get_description();
  my $table_header = get_table_header();
  my @row_data     = get_table_rows();
  my $notes        = get_notes();

  $output  = "{\n";
  $output .= '    "title": "' . $title . '",' . "\n";
  $output .= '    "introduction": ' . $description . ',' . "\n";
  $output .= '    "tableheader": ' . $table_header . ',' . "\n";
  $output .= '    "data": [' . "\n";

  my @rows;
  foreach (@row_data) {
      my($blank, $file, $desc, $version, $result, $aresult) = 0;
      if (/^\|/) {
        ($blank, $file, $desc, $version, $result, $aresult) = split('\|', $_);
        $aresult = "";
      }
      else {
        ($file, $desc, $version, $result, $aresult) = split('\|', $_);
      }
      my $row;
      $row  = '        {' . "\n";
      $row .= '            "filename":    "' . $file    . '",' . "\n";
      $row .= '            "description": "' . $desc    . '",' . "\n";
      $row .= '            "version":     "' . $version . '",' . "\n";
      $row .= '            "results":     ';
      if ($result =~ m/[+\-]?(?:0|[1-9]\d*)(?:\.\d*)?(?:[eE][+\-]?\d+)?/ &&
          $result !~ m/[<>]/ &&
          $result !~ m/^[*]/) {
          $row .= $result  . ",\n";
      }
      else {
          $row .= '"' . $result  . '",' . "\n";
      }
      $row .= '            "analytical":  ';
      if ($aresult =~ m/[+\-]?(?:0|[1-9]\d*)(?:\.\d*)?(?:[eE][+\-]?\d+)?/ &&
          $aresult !~ m/[<>]/ &&
          $aresult !~ m/^[*]/) {
          $row .= $aresult  . "\n";
      }
      else {
          $row .= '"' . $aresult  . '"' . "\n";
      }
      $row .= '        }';
      push(@rows, $row);
  }
  $output .= join(",\n", @rows);
  $output .= "\n" . '    ],' . "\n";
  $output .= '    "notes": ' . $notes . "\n}\n";
  return $output;
}


sub write_output($) {
    my($output) = @_;
    my $output_file = $INPUT_FILE;
    $output_file =~ s/\.md$/.json/;
    open(FD, ">$output_file") || die("Can't create $output_file\n");
    print(FD $output);
    close(FD);
}


sub is_data_file() {
    open(FD, $INPUT_FILE) || die("Can't open $INPUT_FILE\n");
    while(<FD>) {
      if (/^Input File\s*\|/) {
        return 1;
      }
    }
    close(FD);
    return 0;
}


sub main()
{
  usage(1) if (1 == $HELP);

  if (! -f "$INPUT_FILE")
  {
    usage(0);
    croak("ERROR: Can't find input file, $INPUT_FILE: $!\n");
  }
  if (is_data_file()) {
    print("PROCESSING File: $INPUT_FILE\n");
    my $output = get_output();
    write_output($output);
  }
  else {
    print("SKIPPING File: $INPUT_FILE\n");
  }
}


main();

exit(0);