If you're just starting with Gotham's DAO governance, here's a **step-by-step beginner's guide** to participating in decision-making:

---

### **1. Understand Your Role Based on Stake**
| Stake Size | Governance Tier | How to Participate |
|------------|----------------|-------------------|
| **Top 10 Wallets** | The Table | Automatic membership - vote on emergency proposals |
| **Top 100 Wallets** | The Asylum | Automatic membership - vote on all proposals |
| **0.1%+ Stake** | Can run for City Senator | Campaign to be elected by token holders in your region |
| **Any Stake** | General Voter | Delegate votes or join committees |

**Check your status**:  
Run `!gov status [your-wallet-address]` in Gotham's Discord to see your eligibility.

---

### **2. Voting Mechanics Simplified**
#### **For Beginners:**
- **Step 1**: Connect wallet at [gov.gotham.org](https://gov.gotham.org)
- **Step 2**: Choose how to vote:
  - **Direct Voting** (if in The Table/Asylum)
  - **Delegate Votes** (if small holder - pick a senator)
  - **Join a Sub-DAO** (e.g., Arbitration DAO requires 0.05% stake)

#### **Vote Types:**
| Type | Duration | Threshold | Example Use |
|------|---------|-----------|-------------|
| Fast Vote | 24h | 66% Table + 51% Asylum | Emergency bug fixes |
| Standard Vote | 7d | 60% Asylum | New fee structure |
| Committee Vote | 3d | 80% members | Middleman disqualification |

---

### **3. Your First Vote Walkthrough**
**Scenario**: Proposal to reduce dispute resolution time from 24h → 12h

1. **Find Proposals**:
   ```bash
   gotham-cli get-proposals --status=active
   ```
   Output:
   ```
   ID: Prop-42 | Type: Parameter Change | Ends: 2023-12-15
   ```

2. **Read Details**:
   ```bash
   gotham-cli view-proposal Prop-42
   ```

3. **Cast Vote** (Web UI or CLI):
   ```bash
   gotham-cli vote --id=Prop-42 --choice=yes --gas=100000
   ```
   - Requires 0.001 GTH fee to prevent spam

4. **Delegate Instead** (if preferred):
   ```bash
   gotham-cli delegate --to=senator.gth --duration=30d
   ```

---

### **4. Key Commands Cheat Sheet**
| Command | Purpose |
|---------|---------|
| `gov status [addr]` | Check voting power |
| `gov proposals` | List active votes |
| `gov delegate [to]` | Delegate your voting power |
| `gov nominate` | Run for City Senator |
| `gov join-dao [name]` | Join Insurance/Arbitration DAO |

---

### **5. Safety Tips for New Participants**
1. **Start Small**: Begin by delegating votes before direct voting
2. **Verify Proposals**: Always check:
   ```bash
   gotham-cli verify-proposal [id] --check-audit
   ```
3. **Beware of**:
   - Proposals with <24h voting time (likely emergencies)
   - Requests to change treasury addresses

---

### **Where to Learn More**
- **Interactive Tutorial**: `!gov tutorial` in Discord
- **Testnet Practice**: [testnet.gov.gotham.org](https://testnet.gov.gotham.org) (use fake GTH)
- **Weekly Town Hall**: Wednesdays 15:00 UTC in Discord voice chat

**Remember**: Even with 0.001% stake, you can influence decisions by delegating strategically! 🚀