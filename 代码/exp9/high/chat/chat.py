import math
import sys
import time
from data_utils import *
from seq2seq_model import *
from tqdm import tqdm

tf.app.flags.DEFINE_float("learning_rate", 0.001, "Learning rate.")
tf.app.flags.DEFINE_integer("batch_size", 256, "Batch size to use during training.")
tf.app.flags.DEFINE_integer("numEpochs", 30, "Batch size to use during training.")
tf.app.flags.DEFINE_integer("size", 512, "Size of each model layer.")
tf.app.flags.DEFINE_integer("num_layers", 3, "Number of layers in the model.")
tf.app.flags.DEFINE_integer("en_vocab_size", 40000, "English vocabulary size.")
tf.app.flags.DEFINE_integer("en_de_seq_len", 20, "English vocabulary size.")
tf.app.flags.DEFINE_integer("max_train_data_size", 0, "Limit on the size of training data (0: no limit).")
tf.app.flags.DEFINE_integer("steps_per_checkpoint", 100, "How many training steps to do per checkpoint.")
tf.app.flags.DEFINE_string("train_dir", './tmp', "How many training steps to do per checkpoint.")
tf.app.flags.DEFINE_integer("beam_size", 1, "How many training steps to do per checkpoint.")
tf.app.flags.DEFINE_boolean("beam_search", True, "Set to True for beam_search.")
tf.app.flags.DEFINE_boolean("decode", True, "Set to True for interactive decoding.")
FLAGS = tf.app.flags.FLAGS


def create_model(session, forward_only, beam_search, beam_size=5):
    model = Seq2SeqModel(
        FLAGS.en_vocab_size, FLAGS.en_vocab_size, [10, 10],
        FLAGS.size, FLAGS.num_layers, FLAGS.batch_size,
        FLAGS.learning_rate, forward_only=forward_only, beam_search=beam_search, beam_size=beam_size)
    ckpt = tf.train.latest_checkpoint(FLAGS.train_dir)
    model_path = './tmp/chat_bot.ckpt-0'
    if forward_only:
        model.saver.restore(session, model_path)
    elif ckpt and tf.gfile.Exists(ckpt.model_checkpoint_path):
        print("Reading model parameters from %s" % ckpt.model_checkpoint_path)
        model.saver.restore(session, ckpt.model_checkpoint_path)
    else:
        print("Created model with fresh parameters.")
        session.run(tf.initialize_all_variables())
    return model


sess = tf.Session()
data_path = './data/dataset-cornell-length10-filter1-vocabSize40000.pkl'
word2id, id2word, trainingSamples = loadDataset(data_path)
model = create_model(sess, True, beam_search=FLAGS.beam_size, beam_size=FLAGS.beam_search)
model.batch_size = 1


def chat(sentence):
    batch = sentence2enco(sentence, word2id, model.en_de_seq_len)
    beam_path, beam_symbol = model.step(sess, batch.encoderSeqs, batch.decoderSeqs, batch.targetSeqs, batch.weights,goToken)
    paths = []
    curr = 0
    num_steps = len(beam_path)
    for i in range(num_steps - 1, -1, -1):
        paths.append(beam_symbol[i][curr])
        curr = beam_path[i][curr]
    foutputs = [int(logit) for logit in paths[::-1]]
    if eosToken in foutputs:
        foutputs = foutputs[:foutputs.index(eosToken)]
    rec = " ".join([tf.compat.as_str(id2word[output]) for output in foutputs if output in id2word])
    return rec
