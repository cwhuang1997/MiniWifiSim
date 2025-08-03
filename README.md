
# MiniWifiSim - ��� IEEE 802.11 ���ɶ��X�� CSMA/CA ������

## ? �ؿ����c

```
MINI802.11SIM/
�u�w�w .vscode/
�x   �u�w�w settings.json
�x   �u�w�w tasks.json
�u�w�w include/            # �Ҧ����Y�ɡ]�ŧi struct �P��ƭ쫬�^
�x   �u�w�w station.h       # STA ���c�P������ƫŧi
�x   �u�w�w event.h         # �ƥ��C���c�P�ާ@��ƫŧi
�x   �|�w�w logger.h        # �����έp�P log ��ƫŧi
�u�w�w src/                # �{���D���l�X
�x   �u�w�w main.c          # �D�����{���]tick-by-tick ����y�{�^
�x   �u�w�w station.c       # �U STA �����A���P�ǿ��޿��@
�x   �u�w�w event.c         # �ƥ��C���W�R�P�Ƨǹ�@
�x   �|�w�w logger.c        # �έp�O���P������X�榡��
�|�w�w README.md
```

---

## ? �M�׭I���GMiniWiFiSim �O����H

���M�׬��@��²�ƪ� Wi-Fi MAC ��w�������A�D�n���� **IEEE 802.11 Distributed Coordination Function (DCF)** ���� **CSMA/CA�]Carrier Sense Multiple Access with Collision Avoidance�^** ����CDCF �O�ثe Wi-Fi �������̱`�����ǿ鱱�����C

�b���������A�ڭ̫غc�@�������ƥ�t�ΡA�����h�� STA�]Station�^�b�@�ɳq�D�W�i��ʥ]�ǿ�B���� DIFS�B���� Backoff�B�����I���B�B�z ACK ���ާ@�C���M�Ӹ`�w²�ơA�������޿褴����ϬM Wi-Fi ���֤ߴC��s���欰�C

---

## ? ����²��

���������@�ӥH C �y�����g�� IEEE 802.11 DCF (Distributed Coordination Function) ��w�� MAC �h²�Ƽ������A�䴩�G

- CSMA/CA �ǿ�y�{�]�t DIFS, Backoff, SIFS, ACK ���^
- �h STA ���Ҽ����P�v������
- �H���s��Ʋ��ͨƥ�
- �����ɶ��H tick �����A�v tick �B�z�ǰe�P�ƥ�欰

���������i��U�[��U�����ҰѼƹ�� throughput�Bcollision ���į���Ъ��v�T�C

---

## ?? �����]�w�]�T�w�Ѽơ^

- Frame Tx Time         : 3 ticks  
- ACK Tx Time           : 1 tick  
- SIFS                  : 1 tick  
- DIFS                  : 3 ticks  
- Contention Window Min : 7  
- Contention Window Max : 31  
- MAX_RETRY             : 5  
- Total Tick Time       : 100  
- **�C�հѼƼ������榸��**�G3 ���]�������^

---

## ? �������G�K�n�]�����ȡ^

| Case | #STA | NewFrame% | Throughput | Collisions | Busy(%) | Idle(%) | Success/STA | Tx/STA | SuccessRate |
|----------|-----------|----------------|------------|------------|----------------|----------------|--------------------|---------------------|---------------|
| Case A   | 3         | 50%            | 8.66       | 2.33       | 45.33%         | 54.67%         | 2.89               | 3.67                | 80.03%        |
| Case B   | 3         | 90%            | 7.67       | 3.33       | 43.33%         | 56.67%         | 2.55               | 3.78               | 67.90%        |
| Case C   | 10        | 50%            | 5.67       | 13.67      | 40.33%         | 59.67%         | 0.57              | 1.97                | 29.20%        |
| Case D   | 10        | 90%            | 4.67       | 18.00      | 38.00%         | 62.00%         | 0.47              | 2.27               | 20.77%        |

---

## ? ���G���R�`��

### Throughput ��{
- **Case A** �]3 STA, 50%�^ throughput �̰��]8.66�^�A��ܦb�C�t���B�p�W�Һ����U�A����ǿ�Ĳv�̨ΡA�B�I�����v�C�C
- **Case B** �]3 STA, 90%�^ throughput �������� 7.67�A���������}�n���ǡA�N��b�p�W�Һ������A�Y�϶ǰe�ݨD���A�]���|�ߧY�y���į�Y��C
- **Case C�BD** �]10 STA�^ throughput ����U���A�ר�O Case D�]4.67�^�A����j�W�Һ����U�A�Y�K���󰪪� frame ���Ͳv�A�]�L�k���v�]�I���W�v�L���ҾɭP���į�l�ӡC

### �I���P���\�v
- �I���Ʊq **Case A�]2.33�^** í�w�a�ɰ��� **Case D�]18.00�^**�A����H�� STA �ƼW�h�Achannel �v���ܱo�@�P�C
- ���\�v�]Success Rate�^�����U���G
  - Case A�G80.03%
  - Case D�G20.77%
  - �Y�K Case D ���`�ǰe���Ƴ̦h�]22.7 ���^�A�������C���ǰe���\����ҫo�̧C�]�� 20.77%�^�A
  ��ܦb���K�� + ���y�q�U�A�I���l���D�`�Y���A�h�ƶǰe���S�����G�C

### Channel �ϥα���
- **Busy Tick ���** �̰��� Case A�]45.33%�^�A��ܤp�W�Һ��������ǰe�欰�������B�W�ví�w�C
- **Case D** �Ϧ� Busy Tick ���� 38.00%�A�����ǰe���h�A���I���y���F�j�q�u�L�Ħ��Ρv�P�ɶ����O�C

### ��X�[��]�ھڲέp�ƾڡ^

- �b�p�W�Һ����U�]Case A�BB�G3 STA�^�A���� frame ���v���M���L���ɶǰe���ơ]3.67 �� 3.78�^�A���]�I�����ƼW�[�]2.33 �� 3.33�^�Asuccess rate �ϦӤU���]80.03% �� 67.90%�^�A�ɭP throughput �� A ��֡]8.66 �� 7.67�^�C
- �� STA �ƴ��ɦ� 10�]Case C�BD�^�A�Y�ϭӧO�`�I�ǰe�v�����]C�G1.97�FD�G2.27�^�A����ǰe�q�j�T�W�[�Achannel ���οE�P�A�ɭP�I����ɡ]C�G13.67�FD�G18.00�^�Asuccess rate ���O���� 29.20% �P 20.77%�Athroughput �Y�^�� 5.67 �M 4.67�C
- ��X���R��ܡG**���O��@�]���y���į�U��**�A�ӬO�u�`�I�� �� �ǰe�N�@�v�����n�L���ɡA�ɭP�W�e�귽�Q�L�׮��ӡA�ϱo�h�ƶǰe�����ѡA�����i�J�Y�����Ĳv�~�V�C

---

## ? IEEE 802.11 �зǬ۲ũ��ˬd

- ? ���T��@ DIFS �� Backoff �� TX �� SIFS �� ACK
- ? �ǰe�ݵ��ݴC�� idle �í˼� DIFS
- ? ���ѷ|���Ǩýվ� contention window
- ? �ǰe��|�̾ڬO�_���� ACK �i��y�{����
- ? Slot-based �˼ƻP�I���B�z�ҲųW�d

---

## ? ���Ӥu�@�P�X�R��V

### �\����X�R
- RTS/CTS �קK hidden node ���D  
- �䴩�h�q�D�P�h��a�x  
- ���� PIFS �P PCF �Ҧ�  
- ��@ queue buffer �ҫ��]�D��@�ʥ]�^

### �έp�P��ı��
- ��X throughput/collision �H�ɶ��ܤƹϪ�  
- ��ı�Ƽ��� 

### ��ǩʴ���
- ������ӽo�ɶ����P�z�Z�]noise�^  
- �[�J�Z���I��B�ǿ�t�v�ܰʵ����z�h�Ѽ�
