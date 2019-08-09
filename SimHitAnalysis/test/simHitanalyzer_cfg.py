import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('SimHitAnalysis')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/eos/user/r/roy2/data/E9F409FB-BDE6-D649-905A-EE1C58464EC1.root'),
)

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2018_realistic', '')

process.TFileService = cms.Service("TFileService",
                                    fileName = cms.string('simHitplots.root')
)

process.load('TrackerDPG.SimHitAnalysis.SimHitAnalyzer_cfi')
#simhitanalyzer = cms.EDAnalyzer('SimHitAnalysis',
#)
                    
process.p = cms.Path(process.simhitanalyzer)
