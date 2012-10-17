#!/usr/bin/perl

use Cwd;
use File::Copy;
use File::Spec;
use File::Path qw(make_path remove_tree);

use Digest::MD5;

sub banner
{
    print "*****************************************************************\n";
    print "*                 amg_codec_tool testsuite                      *\n";
    print "*****************************************************************\n";
    print "*   WARNING!                                                    *\n";
    print "*   Tests heavily utilize HDD I/O. Be careful with SSDs!        *\n";
    print "*   A possible solution is RAM disk software.                   *\n";
    print "*****************************************************************\n";
}

sub compareMd5
{
    my $testFile = shift;
    my $refFile = shift;
    
    open( REF, $refFile );
    $ctx = Digest::MD5->new;
    $ctx->addfile( REF );
    $digestRef = $ctx->hexdigest;
    close( REF );
    
    open( RESULT, $testFile );
    $ctx = Digest::MD5->new;
    $ctx->addfile( RESULT );
    $digestResult = $ctx->hexdigest;
    close( RESULT );
    
    return ($digestRef == $digestResult);
}

sub testDecodePAC
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "decode_pac");
    make_path( $currTestDataPath );
    my $refFile = File::Spec->catpath( $srcDir, "", "GRAPH0.ARC");
    my $inputFile = File::Spec->catpath( $srcDir, "", "GRAPH0.PAC");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "GRAPH0.ARC");
    my $execString = "$amgBinary --decode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ("$execString") == 0 or die("TEST: [PAC -> ARC] Failed : Unable to decode!");
    
    if( compareMd5( $outputFile, $refFile ))
    {
        print "TEST: [PAC -> ARC] OK\n";
    }
    else
    {
        die "TEST: [PAC -> ARC] FAILED : Checksum mismatch!\n";
    }
}

sub testDecodePAC_New
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "decode_pac_new");
    make_path( $currTestDataPath );
    my $refFile = File::Spec->catpath( $srcDir, "", "GRAPH0.ARC");
    my $inputFile = File::Spec->catpath( $srcDir, "", "GRAPH0_NEW.PAC");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "GRAPH0.ARC");
    my $execString = "$amgBinary --decode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ("$execString") == 0 or die("TEST: [PAC -> ARC] Failed : Unable to decode!");
    
    if( compareMd5( $outputFile, $refFile ))
    {
        print "TEST: [PAC -> ARC] OK\n";
    }
    else
    {
        die "TEST: [PAC -> ARC] FAILED : Checksum mismatch!\n";
    }
}

sub testEncodePAC
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "encode_pac");
    make_path( $currTestDataPath );
    # We use a bit different algo for encoding, so the original PAC will have different checksum
    my $refFile = File::Spec->catpath( $srcDir, "", "GRAPH0_NEW.PAC");
    my $inputFile = File::Spec->catpath( $srcDir, "", "GRAPH0.ARC");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "GRAPH0.PAC");
    my $execString = "$amgBinary --encode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [ARC -> PAC] Failed : Unable to encode!");

    if( compareMd5( $outputFile, $refFile ))
    {
        print "TEST: [ARC -> PAC] OK\n";
    }
    else
    {
        die "TEST: [ARC -> PAC] FAILED : Checksum mismatch!\n";
    }
}

sub testProcessARC
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "process_arc");
    make_path( $currTestDataPath );
    
    # Phase 1 : --decode-arc
    my $inputFile = File::Spec->catpath( $srcDir, "", "GRAPH0.ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "GRAPH0_ARC");
    my $outputXml = File::Spec->catpath( $currTestDataPath, "", "GRAPH0_ARC.XML");
    my $execString = "$amgBinary --decode-arc $inputFile $outputDir $outputXml";
    
    make_path( $outputDir );
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [ARC -> {FILES, XML}] Failed : Unable to decode!");
    
    # Phase 2 : --encode-arc
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "GRAPH0.ARC");
    my $inputDir = $outputDir;
    my $inputXml = $outputXml;
    my $execString = "$amgBinary --encode-arc $inputDir $inputXml $outputFile";
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [{FILES, XML} -> ARC] Failed : Unable to encode!");
    
    if( compareMd5( $outputFile, $inputFile ))
    {
        print "TEST: [ARC -> {FILES, XML} -> ARC] OK\n";
    }
    else
    {
        die "TEST: [ARC -> {FILES, XML} -> ARC] FAILED : Checksum mismatch!\n";
    }
}

sub testProcessIMG
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "process_img");
    make_path( $currTestDataPath );
    
    # Phase 0 : --decode-pac
    my $inputFile = File::Spec->catpath( $srcDir, "", "AMAGAMI.IMG");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.ARC");
    my $execString = "$amgBinary --decode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ("$execString") == 0 or die("TEST: [IMG -> ARC] Failed : Unable to decode!");
    
    # Phase 1 : --extract-img
    my $inputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.ARC");
    my $outputDir = File::Spec->catdir(  $currTestDataPath, "AMAGAMI_IMG");
    my $outputXml = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI_IMG.XML");
    my $execString = "$amgBinary --extract-img $inputFile $outputDir $outputXml";
    
    make_path( $outputDir );
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [IMG -> {FILES, XML}] Failed : Unable to decode!");
    
    # Phase 2 : --pack-img
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.NEW.ARC");
    my $inputDir = $outputDir;
    my $inputXml = $outputXml;
    my $execString = "$amgBinary --pack-img $inputDir $inputXml $outputFile";
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [{FILES, XML} -> IMG] Failed : Unable to encode!");
    
    if( compareMd5( $outputFile, $inputFile ))
    {
        print "TEST:  [IMG -> {FILES, XML} -> IMG] OK\n";
    }
    else
    {
        die "TEST: [IMG -> {FILES, XML} -> IMG] FAILED : Checksum mismatch!\n";
    }
    
    # Phase 3 : --encode-pac
    my $refFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.NEW.ARC");
    my $inputFile = $refFile;
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.NEW.IMG");
    my $execString = "$amgBinary --encode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [ARC -> IMG] Failed : Unable to encode!");
    
    # Phase 4 : --decode-pac
    my $inputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.NEW.IMG");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.NEW.NEW.ARC");
    my $execString = "$amgBinary --decode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [IMG -> ARC] Failed : Unable to encode!");
    
    if( compareMd5( $outputFile, $refFile ))
    {
        print "TEST:  [IMG -> ARC -> IMG] OK\n";
    }
    else
    {
        die "TEST: [IMG -> ARC -> IMG] FAILED : Checksum mismatch!\n";
    }
}

sub testProcessSCF
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "process_scf");
    make_path( $currTestDataPath );
    
    # Phase 1 : --decode-arc
    my $inputFile = File::Spec->catpath( $srcDir, "", "SCENARIO.ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "SCENARIO_ARC");
    my $outputXml = File::Spec->catpath( $currTestDataPath, "", "SCENARIO_ARC.XML");
    my $execString = "$amgBinary --decode-arc $inputFile $outputDir $outputXml";
    
    make_path( $outputDir );
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [ARC -> {FILES, XML}] Failed : Unable to decode!");
    
    # Phase 2 : --decode-scf
    my $inputDir = File::Spec->catdir( $currTestDataPath, "SCENARIO_ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "SCENARIO_ARC_NEW");
    
    make_path( $outputDir );
    
    my $inputDirHndl;
    opendir($inputDirHndl, $inputDir) or die "Cannot open directory!";
    my @inputFiles = readdir( $inputDirHndl);
    closedir( $inputDirHndl);

    foreach my $inFile ( @inputFiles )
    {
       next if( substr($inFile, 0, 1) eq ".");
       
       my $inFullPath = File::Spec->catpath( $inputDir, "", $inFile);
       my $outFullPath = File::Spec->catpath( $outputDir, "", $inFile.".XML");
        
        my $execString = "$amgBinary --decode-scf $inFullPath $outFullPath";
        print "Running: $execString\n";
        system ( "$execString" ) == 0 or die("TEST: [SCF -> XML] Failed : Unable to decode!");
    }
    
    # Phase 3 : --encode-scf
    my $inputDir = File::Spec->catdir( $currTestDataPath, "SCENARIO_ARC_NEW");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "SCENARIO_ARC_NEW_NEW");
    
    make_path( $outputDir );
    
    my $inputDirHndl;
    opendir($inputDirHndl, $inputDir) or die "Cannot open directory!";
    my @inputFiles = readdir( $inputDirHndl);
    closedir( $inputDirHndl);

    foreach my $inFile ( @inputFiles )
    {
       next if( substr($inFile, 0, 1) eq ".");
       
       my $inFullPath = File::Spec->catpath( $inputDir, "", $inFile);
       my $outFullPath = File::Spec->catpath( $outputDir, "", substr($inFile, 0, length($inFile) - 4));
        
        my $execString = "$amgBinary --encode-scf $inFullPath $outFullPath";
        print "Running: $execString\n";
        system ( "$execString" ) == 0 or die("TEST: [XML -> SCF] Failed : Unable to decode!");
    }
    
    # Phase 4 : --encode-arc
    my $refFile = File::Spec->catpath( $srcDir, "", "SCENARIO.ARC");
    my $inputDir = File::Spec->catdir( $currTestDataPath, "SCENARIO_ARC_NEW_NEW");
    my $inputXml = File::Spec->catpath( $currTestDataPath, "", "SCENARIO_ARC.XML");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "SCENARIO.NEW.ARC");
    my $execString = "$amgBinary --encode-arc $inputDir $inputXml $outputFile";

    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [{FILES, XML} -> ARC] Failed : Unable to decode!");
    
    if( compareMd5( $outputFile, $refFile ))
    {
        print "TEST:  [ARC -> SCF -> ARC] OK\n";
    }
    else
    {
        die "TEST: [ARC -> SCF -> ARC] FAILED : Checksum mismatch!\n";
    }
}

sub testProcessSCF_IMG
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "process_scf_img");
    make_path( $currTestDataPath );
    
    # Phase 0 : --decode-pac
    my $inputFile = File::Spec->catpath( $srcDir, "", "AMAGAMI.IMG");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.ARC");
    my $execString = "$amgBinary --decode-pac $inputFile $outputFile";
    
    print "Running: $execString\n";
    system ("$execString") == 0 or die("TEST: [IMG -> ARC] Failed : Unable to decode!");
    
    # Phase 1 : --extract-img
    my $inputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "AMAGAMI_ARC");
    my $outputXml = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI_ARC.XML");
    my $execString = "$amgBinary --extract-img $inputFile $outputDir $outputXml";
    
    make_path( $outputDir );
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [ARC -> {FILES, XML}] Failed : Unable to decode!");
    
    # Phase 2 : --decode-scf
    my $inputDir = File::Spec->catdir( $currTestDataPath, "AMAGAMI_ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "AMAGAMI_ARC_NEW");
    
    make_path( $outputDir );
    
    my $inputDirHndl;
    opendir($inputDirHndl, $inputDir) or die "Cannot open directory!";
    my @inputFiles = readdir( $inputDirHndl);
    closedir( $inputDirHndl);

    foreach my $inFile ( @inputFiles )
    {
       next if( substr($inFile, 0, 1) eq ".");
       
       my $inFullPath = File::Spec->catpath( $inputDir, "", $inFile);
       my $outFullPath = File::Spec->catpath( $outputDir, "", $inFile.".XML");
        
        my $execString = "$amgBinary --decode-scf $inFullPath $outFullPath";
        print "Running: $execString\n";
        system ( "$execString" ) == 0 or die("TEST: [SCF -> XML] Failed : Unable to decode!");
    }
    
    # Phase 3 : --encode-scf
    my $inputDir = File::Spec->catdir( $currTestDataPath, "AMAGAMI_ARC_NEW");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "AMAGAMI_ARC_NEW_NEW");
    
    make_path( $outputDir );
    
    my $inputDirHndl;
    opendir($inputDirHndl, $inputDir) or die "Cannot open directory!";
    my @inputFiles = readdir( $inputDirHndl);
    closedir( $inputDirHndl);

    foreach my $inFile ( @inputFiles )
    {
       next if( substr($inFile, 0, 1) eq ".");
       
       my $inFullPath = File::Spec->catpath( $inputDir, "", $inFile);
       my $outFullPath = File::Spec->catpath( $outputDir, "", substr($inFile, 0, length($inFile) - 4));
        
        my $execString = "$amgBinary --encode-scf $inFullPath $outFullPath";
        print "Running: $execString\n";
        system ( "$execString" ) == 0 or die("TEST: [XML -> SCF] Failed : Unable to decode!");
    }
    
    # Phase 4 : --pack-img
    my $refFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.ARC");
    my $inputDir = File::Spec->catdir( $currTestDataPath, "AMAGAMI_ARC_NEW_NEW");
    my $inputXml = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI_ARC.XML");
    my $outputFile = File::Spec->catpath( $currTestDataPath, "", "AMAGAMI.NEW.ARC");
    my $execString = "$amgBinary --pack-img $inputDir $inputXml $outputFile";

    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [{FILES, XML} -> ARC] Failed : Unable to decode!");
    
    if( compareMd5( $outputFile, $refFile ))
    {
        print "TEST:  [ARC -> SCF -> ARC] OK\n";
    }
    else
    {
        die "TEST: [ARC -> SCF -> ARC] FAILED : Checksum mismatch!\n";
    }
}

sub testAlphaFix
{
    my $currTestDataPath = File::Spec->catdir( $tmpDir, "alpha_fix");
    make_path( $currTestDataPath );
    
    # Phase 0 : --decode-arc
    my $inputFile = File::Spec->catpath( $srcDir, "", "GRAPH0.ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "GRAPH0_ARC");
    my $outputXml = File::Spec->catpath( $currTestDataPath, "", "GRAPH0_ARC.XML");
    my $execString = "$amgBinary --decode-arc $inputFile $outputDir $outputXml";
    
    make_path( $outputDir );
    
    print "Running: $execString\n";
    system ( "$execString" ) == 0 or die("TEST: [ARC -> {FILES, XML}] Failed : Unable to decode!");
    
    # Phase 1 : --alphafix -d then --alphafix -u
    my $inputDir = File::Spec->catdir( $currTestDataPath, "GRAPH0_ARC");
    my $outputDir = File::Spec->catdir( $currTestDataPath, "GRAPH0_ARC_NEW");
    
    make_path( $outputDir );
    
    my $inputDirHndl;
    opendir($inputDirHndl, $inputDir) or die "Cannot open directory!";
    my @inputFiles = readdir( $inputDirHndl);
    closedir( $inputDirHndl);

    foreach my $inFile ( @inputFiles )
    {
        next if( substr($inFile, 0, 1) eq ".");

        # Phase 1a : --alphafix -d
        my $inFullPath = File::Spec->catpath( $inputDir, "", $inFile);
        my $outFullPath = File::Spec->catpath( $outputDir, "", $inFile.".down");
        my $refFullPath = File::Spec->catpath( $inputDir, "", $inFile);
        
        copy($inFullPath, $outFullPath);
        
        my $execString = "$amgBinary --alphafix -d $outFullPath";
        print "Running: $execString\n";
        system ( "$execString" ) == 0 or die("TEST: [TM2 downsampling] Failed : Unable to process!");
        
        # Phase 1b : --alphafix -u
        my $inFullPath = File::Spec->catpath( $inputDir, "", $inFile);
        my $outFullPath = File::Spec->catpath( $outputDir, "", $inFile.".up");
        
        copy($inFullPath, $outFullPath);
        
        my $execString = "$amgBinary --alphafix -u $outFullPath";
        print "Running: $execString\n";
        system ( "$execString" ) == 0 or die("TEST: [TM2 upsampling] Failed : Unable to process!");
        
        # TODO: any verification method like MD5?
    }
}

# 0. Show banner
banner();

# Look around
my @splitCwd = File::Spec->splitdir( cwd());
$workDir = File::Spec->catdir( @splitCwd);
$binDir  = File::Spec->catdir( $workDir, bin);
$srcDir  = File::Spec->catdir( $workDir, src);
$tmpDir  = File::Spec->catdir( $workDir, tmp);

$amgBinary  = File::Spec->catpath( $binDir, "", "amg_codec_tool.exe");
# Temp workaround for DLL lookup and other tuning
$amgBinary = File::Spec->catpath( $binDir, "", "runme.bat > nul") . " && " . $amgBinary;

print "Workdir : $workDir\n";
print "Binaries dir : $binDir\n";
print "Source files dir : $srcDir\n";
print "Temporary files dir : $tmpDir\n";
print "AMG binary path : $amgBinary\n";

#system( "$amgBinary" );

remove_tree( $tmpDir );
make_path( $tmpDir );

# ---------------------------------------------------------
# Now each test is standalone, thus can be run separately.
# ---------------------------------------------------------

# 1. Check PAC<->ARC transcoding
# 1a. Check --decode-pac mode
#testDecodePAC();
# 1b. Check --encode-pac mode
#testEncodePAC();
# 1c. Check --decode-pac mode again
#testDecodePAC_New();

# 2. Check ARC unpack/repack
# 2a. Check --decode-arc mode and then --encode-arc mode
#testProcessARC();

# 3. Check IMG unpack/repack
# 3a. Check --extract-img mode and then --pack-img mode
#testProcessIMG();

# 4. Check SCF unpack/repack
# 4a. Convert ARC to SCF, then SCF to XML, then XML back to SCF, then SCF to ARC
#testProcessSCF();
# 4b. Convert IMG to ARC, then ARC to SCF, then SCF to XML, then XML back to SCF, then SCF to ARC
#testProcessSCF_IMG();

# 5. Check Image processing
# 5a. Apply --alphafix, then revert
#testAlphaFix();

