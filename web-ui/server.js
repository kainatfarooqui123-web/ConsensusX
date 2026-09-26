/**
 * ConsensusX Web UI - Backend Server
 * ===================================
 * DATABASE CONNECTION IS HERE (lines ~25-35)
 * This file connects to MongoDB using Mongoose.
 */

require('dotenv').config();
const express = require('express');
const cors = require('cors');
const mongoose = require('mongoose');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// --- Middleware ---
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// --- MongoDB Schema (what we store) ---
const sequenceSchema = new mongoose.Schema({
  name: { type: String, required: true },
  sequence: { type: String, required: true },
  notes: { type: String, default: '' },
  createdAt: { type: Date, default: Date.now }
});

const Sequence = mongoose.model('Sequence', sequenceSchema);

// =============================================
// DATABASE CONNECTION (MongoDB)
// =============================================
const MONGODB_URI = process.env.MONGODB_URI || 'mongodb://127.0.0.1:27017/consensusx';

async function connectDatabase() {
  try {
    await mongoose.connect(MONGODB_URI);
    console.log('✓ Connected to MongoDB');
    console.log('  URI:', MONGODB_URI.replace(/\/\/.*@/, '//<credentials>@')); // hide password in logs
  } catch (err) {
    console.error('✗ MongoDB connection failed:', err.message);
    console.error('  Make sure MongoDB is running and MONGODB_URI in .env is correct.');
    process.exit(1);
  }
}

// --- API Routes ---

// GET all sequences
app.get('/api/sequences', async (req, res) => {
  try {
    const sequences = await Sequence.find().sort({ createdAt: -1 });
    res.json(sequences);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// POST new sequence
app.post('/api/sequences', async (req, res) => {
  try {
    const { name, sequence, notes } = req.body;
    if (!name || !sequence) {
      return res.status(400).json({ error: 'Name and sequence are required' });
    }
    const record = new Sequence({ name, sequence: sequence.toUpperCase(), notes: notes || '' });
    await record.save();
    res.status(201).json(record);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// DELETE a sequence
app.delete('/api/sequences/:id', async (req, res) => {
  try {
    await Sequence.findByIdAndDelete(req.params.id);
    res.json({ message: 'Deleted' });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Health check
app.get('/api/health', (req, res) => {
  res.json({
    status: 'ok',
    database: mongoose.connection.readyState === 1 ? 'connected' : 'disconnected'
  });
});

// --- Start server ---
connectDatabase().then(() => {
  app.listen(PORT, () => {
    console.log(`✓ Server running at http://localhost:${PORT}`);
  });
});
