/**
 * ConsensusX Web UI - Frontend
 * Talks to the backend API (server.js) which saves data in MongoDB.
 */

const API = '/api';

const form = document.getElementById('sequenceForm');
const messageEl = document.getElementById('message');
const listEl = document.getElementById('sequenceList');
const statusEl = document.getElementById('status');

let cachedSequences = [];

// Load sequences when page opens
loadSequences();

// Handle form submit
form.addEventListener('submit', async (e) => {
  e.preventDefault();
  hideMessage();

  const name = document.getElementById('name').value.trim();
  const sequence = document.getElementById('sequence').value.trim();
  const notes = document.getElementById('notes').value.trim();

  try {
    const res = await fetch(`${API}/sequences`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name, sequence, notes })
    });

    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Failed to save');

    showMessage('Sequence saved to MongoDB!', 'success');
    form.reset();
    loadSequences();
  } catch (err) {
    showMessage(err.message, 'error');
  }
});

// Fetch and display all sequences
async function loadSequences() {
  statusEl.textContent = 'Loading...';
  listEl.innerHTML = '';

  try {
    const res = await fetch(`${API}/sequences`);
    const sequences = await res.json();

    if (!res.ok) throw new Error(sequences.error || 'Failed to load');

    cachedSequences = sequences;
    statusEl.textContent = `${sequences.length} sequence(s) in database`;

    if (sequences.length === 0) {
      listEl.innerHTML = '<li class="empty">No sequences yet. Add one above!</li>';
      return;
    }

    listEl.innerHTML = sequences.map((s) => `
      <li data-id="${s._id}">
        <div class="name">${escapeHtml(s.name)}</div>
        <div class="seq">${escapeHtml(s.sequence)}</div>
        ${s.notes ? `<div class="meta">${escapeHtml(s.notes)}</div>` : ''}
        <div class="meta">${new Date(s.createdAt).toLocaleString()}</div>
        <button class="delete-btn" onclick="deleteSequence('${s._id}')">Delete</button>
      </li>
    `).join('');
  } catch (err) {
    statusEl.textContent = 'Could not connect to server. Is it running?';
    listEl.innerHTML = `<li class="empty">${escapeHtml(err.message)}</li>`;
  }
}

// Delete a sequence
async function deleteSequence(id) {
  if (!confirm('Delete this sequence?')) return;

  try {
    const res = await fetch(`${API}/sequences/${id}`, { method: 'DELETE' });
    if (!res.ok) throw new Error('Delete failed');
    loadSequences();
  } catch (err) {
    showMessage(err.message, 'error');
  }
}

function showMessage(text, type) {
  messageEl.textContent = text;
  messageEl.className = `message ${type}`;
}

function hideMessage() {
  messageEl.className = 'message';
  messageEl.textContent = '';
}

function escapeHtml(str) {
  const div = document.createElement('div');
  div.textContent = str;
  return div.innerHTML;
}

// =============================================
// SPECIFIC NUCLEOTIDE SEQUENCE FINDER
// =============================================

function find_sequence(full_sequence, query_sequence) {
  const full = full_sequence.replace(/[\s\r\n\t]/g, '').toUpperCase();
  const query = query_sequence.replace(/[\s\r\n\t]/g, '').toUpperCase();
  if (!full || !query) {
    return { present: false, positions: [] };
  }
  const positions = [];
  let idx = 0;
  while (idx <= full.length - query.length) {
    const found = full.indexOf(query, idx);
    if (found === -1) break;
    positions.push(found + 1); // 1-based biological numbering
    idx = found + 1; // allows overlapping matches
  }
  return {
    present: positions.length > 0,
    positions: positions
  };
}

function parseInputSequences(raw) {
  if (!raw || !raw.trim()) return [];

  const lines = raw.split(/\r?\n/);
  const hasFastaHeader = lines.some(l => l.trim().startsWith('>'));
  const hasLabeledHeader = lines.some(l => /^(sequence|seq|sample)\s*\d*.*:/i.test(l.trim()));

  if (hasFastaHeader) {
    const sequences = [];
    let currentName = '';
    let currentSeq = '';

    for (const line of lines) {
      const trimmed = line.trim();
      if (trimmed.startsWith('>')) {
        if (currentName || currentSeq) {
          sequences.push({
            name: currentName || `Sequence ${sequences.length + 1}`,
            sequence: currentSeq
          });
        }
        currentName = trimmed.slice(1).trim();
        currentSeq = '';
      } else {
        currentSeq += trimmed.replace(/\s+/g, '');
      }
    }
    if (currentName || currentSeq) {
      sequences.push({
        name: currentName || `Sequence ${sequences.length + 1}`,
        sequence: currentSeq
      });
    }
    return sequences;
  }

  if (hasLabeledHeader) {
    const sequences = [];
    let currentName = '';
    let currentSeq = '';

    for (const line of lines) {
      const trimmed = line.trim();
      if (/^(sequence|seq|sample)\s*\d*.*:/i.test(trimmed)) {
        if (currentName || currentSeq) {
          sequences.push({
            name: currentName || `Sequence ${sequences.length + 1}`,
            sequence: currentSeq
          });
        }
        const colonIdx = trimmed.indexOf(':');
        currentName = trimmed.slice(0, colonIdx).trim();
        const rest = trimmed.slice(colonIdx + 1).trim();
        currentSeq = rest.replace(/\s+/g, '');
      } else {
        currentSeq += trimmed.replace(/\s+/g, '');
      }
    }
    if (currentName || currentSeq) {
      sequences.push({
        name: currentName || `Sequence ${sequences.length + 1}`,
        sequence: currentSeq
      });
    }
    return sequences;
  }

  const cleaned = raw.replace(/[\s\r\n\t]/g, '').toUpperCase();
  return [{ name: '', sequence: cleaned }];
}

function validateNucleotides(seq) {
  for (let i = 0; i < seq.length; i++) {
    const c = seq[i];
    if (!'ACGTU'.includes(c)) {
      return `Invalid character '${c}' at position ${i + 1}. Valid characters: A, C, G, T, U.`;
    }
  }
  return null;
}

const findBtn = document.getElementById('findBtn');
const findQueryEl = document.getElementById('findQuery');
const findMessageEl = document.getElementById('findMessage');
const findResultsEl = document.getElementById('findResults');

if (findBtn) {
  findBtn.addEventListener('click', () => {
    findMessageEl.textContent = '';
    findMessageEl.className = 'message';
    findResultsEl.style.display = 'none';
    findResultsEl.innerHTML = '';

    const rawQuery = (findQueryEl.value || '').trim();
    const query = rawQuery.replace(/[\s\r\n\t]/g, '').toUpperCase();
    if (!query) {
      findMessageEl.textContent = 'Error: Please enter a sequence to find.';
      findMessageEl.className = 'message error';
      return;
    }

    const queryErr = validateNucleotides(query);
    if (queryErr) {
      findMessageEl.textContent = `Error in search sequence: ${queryErr}`;
      findMessageEl.className = 'message error';
      return;
    }

    let targets = [];
    if (cachedSequences && cachedSequences.length > 0) {
      targets = cachedSequences.map(s => ({
        name: s.name,
        sequence: s.sequence
      }));
    } else {
      const inputSeq = document.getElementById('sequence').value;
      if (inputSeq && inputSeq.trim()) {
        targets = parseInputSequences(inputSeq);
        const inputName = document.getElementById('name').value.trim();
        if (targets.length === 1 && !targets[0].name && inputName) {
          targets[0].name = inputName;
        }
      }
    }

    if (targets.length === 0) {
      findMessageEl.textContent = 'Error: No sequences available. Enter or save a sequence first.';
      findMessageEl.className = 'message error';
      return;
    }

    for (let i = 0; i < targets.length; i++) {
      const t = targets[i];
      const seqErr = validateNucleotides(t.sequence.toUpperCase());
      if (seqErr) {
        const label = t.name || `Sequence ${i + 1}`;
        findMessageEl.textContent = `Error in ${label}: ${seqErr}`;
        findMessageEl.className = 'message error';
        return;
      }
    }

    const isMultiple = targets.length > 1 || Boolean(targets[0].name);

    if (!isMultiple) {
      const res = find_sequence(targets[0].sequence, query);
      if (res.present) {
        const posText = res.positions.length === 1
          ? `Position: ${res.positions[0]}`
          : `Positions: ${res.positions.join(', ')}`;
        findResultsEl.innerHTML = `<span class="finder-badge found">Present — ${posText}</span>`;
      } else {
        findResultsEl.innerHTML = `<span class="finder-badge not-found">Not Present</span>`;
      }
    } else {
      let html = '';
      for (let i = 0; i < targets.length; i++) {
        const t = targets[i];
        const label = t.name || `Sequence ${i + 1}`;
        const res = find_sequence(t.sequence, query);
        if (res.present) {
          const posText = res.positions.length === 1
            ? `Position: ${res.positions[0]}`
            : `Positions: ${res.positions.join(', ')}`;
          html += `
            <div class="finder-result-item">
              <strong>${escapeHtml(label)}</strong>:
              <span class="finder-badge found" style="margin-left:0.5rem;">Present — ${posText}</span>
            </div>
          `;
        } else {
          html += `
            <div class="finder-result-item">
              <strong>${escapeHtml(label)}</strong>:
              <span class="finder-badge not-found" style="margin-left:0.5rem;">Not Present</span>
            </div>
          `;
        }
      }
      findResultsEl.innerHTML = html;
    }

    findResultsEl.style.display = 'block';
  });
}

