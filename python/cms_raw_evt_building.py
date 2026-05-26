import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import glob
from pathlib import Path
  
options = VarParsing.VarParsing()  
  
options.register('raw_directory',  
    '',  
    VarParsing.VarParsing.multiplicity.singleton,  
    VarParsing.VarParsing.varType.string,  
    "Directory containing raw files"  
)  
  
options.register('output_path',  
    '',  
    VarParsing.VarParsing.multiplicity.singleton,  
    VarParsing.VarParsing.varType.string,  
    "Path for output converted root file"  
)  
  
options.register('run_number',  
    0,  
    VarParsing.VarParsing.multiplicity.singleton,  
    VarParsing.VarParsing.varType.int,  
    "Run number"  
)  
  
options.parseArguments()  
  
if not options.raw_directory or not options.output_path or not options.run_number:  
    raise ValueError("rawDir, outputDir, and runNumber must all be provided")


process = cms.Process("Convert")
process.load("DQM.SiStripCommon.MessageLogger_cfi")

process.EvFDaqDirector = cms.Service("EvFDaqDirector",
    baseDir   = cms.untracked.string(options.raw_directory),
    buBaseDir = cms.untracked.string(options.raw_directory),
    runNumber = cms.untracked.uint32(options.run_number),
)

process.FastMonitoringService = cms.Service("FastMonitoringService",
    filePerFwkStream = cms.untracked.bool(False),
    fastMonIntervals = cms.untracked.uint32(2),
)

raw_files = sorted(glob.glob(Path(options.raw_directory) / "*index*.raw"))
raw_files = ["file:" + f for f in raw_files]
print(raw_files)

process.source = cms.Source("FedRawDataInputSource",
    fileNames        = cms.untracked.vstring(raw_files),
    verifyAdler32    = cms.untracked.bool(False),
    useL1EventID     = cms.untracked.bool(True),
    keepRawFiles  = cms.untracked.bool(True),
)

maxEvents = cms.PSet(input = cms.untracked.int32(-1))

process.out = cms.OutputModule("PoolOutputModule",
    fileName       = cms.untracked.string(options.ouput_path),
    outputCommands = cms.untracked.vstring("drop *", "keep FEDRawDataCollection_*_*_*"),
)

process.e = cms.EndPath(process.out)