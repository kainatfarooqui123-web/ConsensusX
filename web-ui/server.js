web-ui/server.js
require('dotenv').config();

const express = require('express');
const cors = require('cors');
const mongoose = require('mongoose');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

// Useful when running locally
app.use(express.static(path.join(__dirname, 'public')));

// MongoDB model
const sequenceSchema = new mongoose.Schema({
  name: { type: String, required: true },
  sequence: { type: String, required: true },
  notes: { type: String, default: '' },
  createdAt: { type: Date, default: Date.now }
});

const Sequence =
  mongoose.models.Sequence || mongoose.model('Sequence', sequenceSchema);

// MongoDB connection
const MONGODB_URI = process.env.MONGODB_URI;

let connectionPromise = null;

async function connectDatabase() {
  if (mongoose.connection.readyState === 1) {
    return;
  }

  if (!MONGODB_URI) {
    throw new Error('MONGODB_URI environment variable is missing');
  }

  if (!connectionPromise) {
    connectionPromise = mongoose.connect(MONGODB_URI).catch((err) => {
      connectionPromise = null;
      throw err;
    });
  }

  await connectionPromise;
}

// Connect MongoDB before API requests
app.use('/api', async (req, res, next) => {
  try {
    await connectDatabase();
    next();
  } catch (err) {
    console.error('MongoDB connection error:', err.message);

    res.status(500).json({
      error: 'Database connection failed'
    });
  }
});

// Get all sequences
app.get('/api/sequences', async (req, res) => {
  try {
    const sequences = await Sequence.find().sort({ createdAt: -1 });
    res.json(sequences);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Save sequence
app.post('/api/sequences', async (req, res) => {
  try {
    const { name, sequence, notes } = req.body;

    if (!name || !sequence) {
      return res.status(400).json({
        error: 'Name and sequence are required'
      });
    }

    const record = new Sequence({
      name,
      sequence: sequence.toUpperCase(),
      notes: notes || ''
    });

    await record.save();

    res.status(201).json(record);

  } catch (err) {
    res.status(500).json({
      error: err.message
    });
  }
});

// Delete sequence
app.delete('/api/sequences/:id', async (req, res) => {
  try {
    await Sequence.findByIdAndDelete(req.params.id);

    res.json({
      message: 'Deleted'
    });

  } catch (err) {
    res.status(500).json({
      error: err.message
    });
  }
});

// Health check
app.get('/api/health', (req, res) => {
  res.json({
    status: 'ok',
    database:
      mongoose.connection.readyState === 1
        ? 'connected'
        : 'disconnected'
  });
});

// IMPORTANT FOR VERCEL
module.exports = app;

// Run normally on your computer
if (require.main === module) {
  connectDatabase()
    .then(() => {
      app.listen(PORT, () => {
        console.log(`Server running at http://localhost:${PORT}`);
      });
    })
    .catch((err) => {
      console.error('Could not start server:', err.message);
    });
}