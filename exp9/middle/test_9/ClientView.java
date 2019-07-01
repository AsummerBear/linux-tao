package com.test_9;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;

public class ClientView {
    private JFrame frame;
    private JButton sendButton;
    private JTextField portText;
    private JTextField ipText;
    private JTextField nameText;
    private JButton control;
    private JTextField content;
    private JTextArea msgList;
    private JPanel panel;
    private JList userList;

    private DefaultListModel listModel;
    private boolean isConnected = false;

    private Socket socket;
    private PrintWriter writer;
    private BufferedReader reader;
    private ClientView.MessageThread messageThread;         // 负责接收消息的线程
    private Map<String, User> onLineUsers = new HashMap<>();// 所有在线用户

    public static void main(String[] args) {
        new ClientView();
    }

    public ClientView(){
        frame = new JFrame("Client");
        listModel = new DefaultListModel();
        userList.setModel(listModel);
        frame.setContentPane(panel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setSize(600,400);
        frame.setVisible(true);

        // 写消息的文本框中按回车键时事件
        content.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                send();
            }
        });

        // 单击发送按钮时事件
        sendButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                send();
            }
        });

        // 单击控制按钮时事件
        control.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int port;
                if (!isConnected) {
                    try {
                        try {
                            port = Integer.parseInt(portText.getText().trim());
                        } catch (NumberFormatException e2) {
                            throw new Exception("端口号不符合要求!端口为整数!");
                        }
                        String hostIp = ipText.getText().trim();
                        String name = nameText.getText().trim();
                        if (name.equals("") || hostIp.equals("")) {
                            throw new Exception("姓名、服务器IP不能为空!");
                        }
                        boolean flag = connectServer(port, hostIp, name);
                        if (flag == false) {
                            throw new Exception("与服务器连接失败!");
                        }
                        frame.setTitle(name);
                        control.setText("断开");
                        portText.setEditable(false);
                        ipText.setEditable(false);
                        nameText.setEditable(false);
                        JOptionPane.showMessageDialog(frame, "成功连接!");
                    } catch (Exception exc) {
                        JOptionPane.showMessageDialog(frame, exc.getMessage(),
                                "错误", JOptionPane.ERROR_MESSAGE);
                    }
                }else {
                    try {
                        boolean flag = closeConnection();// 断开连接
                        if (flag == false) {
                            throw new Exception("断开连接发生异常！");
                        }
                        JOptionPane.showMessageDialog(frame, "成功断开!");
                        listModel.removeAllElements();
                        control.setText("连接");
                        portText.setEditable(true);
                        ipText.setEditable(true);
                        nameText.setEditable(true);
                    } catch (Exception exc) {
                        JOptionPane.showMessageDialog(frame, exc.getMessage(),
                                "错误", JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
        });

        // 关闭窗口时事件
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                if (isConnected) {
                    closeConnection();  // 关闭连接
                }
                System.exit(0);  // 退出程序
            }
        });

    }

    // 执行发送
    public void send() {
        if (!isConnected) {
            JOptionPane.showMessageDialog(frame, "还没有连接服务器，无法发送消息！", "错误", JOptionPane.ERROR_MESSAGE);
            return;
        }
        String message = content.getText().trim();
        if (message == null || message.equals("")) {
            JOptionPane.showMessageDialog(frame, "消息不能为空！", "错误", JOptionPane.ERROR_MESSAGE);
            return;
        }
        sendMessage(frame.getTitle() + "@" + "ALL" + "@" + message);
        content.setText(null);
    }
    /**
     * 连接服务器
     *
     * @param port
     * @param hostIp
     * @param name
     */
    public boolean connectServer(int port, String hostIp, String name) {
        // 连接服务器
        try {
            socket = new Socket(hostIp, port);// 根据端口号和服务器ip建立连接
            writer = new PrintWriter(socket.getOutputStream());
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            // 发送客户端用户基本信息(用户名和ip地址)
            sendMessage(name + "@" + socket.getLocalAddress().toString());
            // 开启接收消息的线程
            messageThread = new MessageThread(reader, msgList);
            messageThread.start();
            isConnected = true; // 已经连接上了
            return true;
        } catch (Exception e) {
            msgList.append("与端口号为：" + port + "    IP地址为：" + hostIp + "   的服务器连接失败!" + "\r\n");
            isConnected = false;   // 未连接上
            return false;
        }
    }

    /**
     * 发送消息
     * @param message
     */
    public void sendMessage(String message) {
        writer.println(message);
        writer.flush();
    }

    /**
     * 客户端主动关闭连接
     */
    @SuppressWarnings("deprecation")
    public synchronized boolean closeConnection() {
        try {
            sendMessage("CLOSE");// 发送断开连接命令给服务器
            messageThread.stop();// 停止接受消息线程
            // 释放资源
            if (reader != null) {
                reader.close();
            }
            if (writer != null) {
                writer.close();
            }
            if (socket != null) {
                socket.close();
            }
            isConnected = false;
            return true;
        } catch (IOException e1) {
            e1.printStackTrace();
            isConnected = true;
            return false;
        }
    }

    // 不断接收消息的线程
    class MessageThread extends Thread {
        private BufferedReader reader;
        private JTextArea msgList;

        // 接收消息线程的构造方法
        public MessageThread(BufferedReader reader, JTextArea msgList) {
            this.reader = reader;
            this.msgList = msgList;
        }

        // 被动的关闭连接
        public synchronized void closeCon() throws Exception {
            // 清空用户列表
            listModel.removeAllElements();
            // 被动的关闭连接释放资源
            if (reader != null) {
                reader.close();
            }
            if (writer != null) {
                writer.close();
            }
            if (socket != null) {
                socket.close();
            }
            isConnected = false;// 修改状态为断开
        }

        public void run() {
            String message = "";
            while (true) {
                try {
                    message = reader.readLine();
                    StringTokenizer stringTokenizer = new StringTokenizer(message, "/@");
                    String command = stringTokenizer.nextToken();// 命令
                    if (command.equals("CLOSE"))// 服务器已关闭命令
                    {
                        msgList.append("服务器已关闭!\r\n");
                        closeCon();// 被动的关闭连接
                        return;// 结束线程
                    } else if (command.equals("ADD")) {// 有用户上线更新在线列表
                        String username = "";
                        String userIp = "";
                        if ((username = stringTokenizer.nextToken()) != null && (userIp = stringTokenizer.nextToken()) != null) {
                            User user = new User(username, userIp);
                            onLineUsers.put(username, user);
                            listModel.addElement(username);
                        }
                    } else if (command.equals("DELETE")) {// 有用户下线更新在线列表
                        String username = stringTokenizer.nextToken();
                        User user = (User) onLineUsers.get(username);
                        onLineUsers.remove(user);
                        listModel.removeElement(username);
                    } else if (command.equals("USERLIST")) {// 加载在线用户列表
                        int size = Integer.parseInt(stringTokenizer.nextToken());
                        String username = null;
                        String userIp = null;
                        for (int i = 0; i < size; i++) {
                            username = stringTokenizer.nextToken();
                            userIp = stringTokenizer.nextToken();
                            User user = new User(username, userIp);
                            onLineUsers.put(username, user);
                            listModel.addElement(username);
                        }
                    } else if (command.equals("MAX")) {// 人数已达上限
                        msgList.append(stringTokenizer.nextToken() + stringTokenizer.nextToken() + "\r\n");
                        closeCon();// 被动的关闭连接
                        JOptionPane.showMessageDialog(frame, "服务器缓冲区已满！", "错误", JOptionPane.ERROR_MESSAGE);
                        return;// 结束线程
                    } else {// 普通消息
                        msgList.append(message + "\r\n");
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
    
}
