import { createServer } from 'http';
import { existsSync, statSync, readFile } from 'fs';
import { fileURLToPath } from 'url';
import { join, normalize, dirname, extname } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const PORT = 3000;
const BASE_DIR = join(__dirname, 'public');

// MIME types mapping
const MIME_TYPES = {
  '.html': 'text/html',
  '.css': 'text/css',
  '.js': 'application/javascript',
  '.wasm': 'application/wasm',
  '.data': 'application/octet-stream',
  '.json': 'application/json',
  '.txt': 'text/plain',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.jpeg': 'image/jpeg',
  '.gif': 'image/gif',
  '.svg': 'image/svg+xml',
};

// Simple logging function
const logRequest = (req, statusCode, filePath = '') => {
  const timestamp = new Date().toISOString();
  const method = req.method;
  const url = req.url;
  const ip = req.socket.remoteAddress;
  const logEntry = `${timestamp} [${method}] ${ip} ${url} ${statusCode} ${filePath}`;
  console.log(logEntry);
};

createServer((req, res) => {
  try {
    const safePath = normalize(decodeURIComponent(req.url)).replace(/^(\.\.[\/\\])+/, '');
    let filePath = join(BASE_DIR, safePath);

    // Check if path exists and is a directory
    if (existsSync(filePath) && statSync(filePath).isDirectory()) {
      filePath = join(filePath, 'index.html');
    }

    // Check if file exists before attempting to read
    if (!existsSync(filePath) || !statSync(filePath).isFile()) {
      res.writeHead(404, { 'Content-Type': 'text/plain' });
      res.end('404 Not Found\n');
      logRequest(req, 404, filePath);
      return;
    }

    readFile(filePath, (err, data) => {
      if (err) {
        res.writeHead(500, { 'Content-Type': 'text/plain' });
        res.end('Internal Server Error\n');
        logRequest(req, 500, filePath);
        return;
      }

      // Get file extension and set appropriate MIME type
      const ext = extname(filePath).toLowerCase();
      const contentType = MIME_TYPES[ext] || 'application/octet-stream';

      res.writeHead(200, { 'Content-Type': contentType });
      res.end(data);
      logRequest(req, 200, filePath);
    });
  } catch (err) {
    res.writeHead(500, { 'Content-Type': 'text/plain' });
    res.end('Internal Server Error\n');
    logRequest(req, 500);
  }
}).listen(PORT, () => {
  console.log(`Serving from /public at http://localhost:${PORT}`);
});
